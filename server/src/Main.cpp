#include "include/DateTime.hpp"
#include <MessageCache.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <libusockets.h>
#include <memory>
#include <string_view>
#include <uWebSockets/App.h>
#include <uWebSockets/HttpParser.h>
#include <uWebSockets/HttpResponse.h>
#include <uWebSockets/WebSocket.h>

struct PerSocketData
{
    std::chrono::system_clock::time_point created_at;
    std::string username;
};

std::string full_message(PerSocketData *&user_data, std::string_view message)
{
    std::cout << user_data->username << ": " << message << std::endl;

    std::stringstream ss;
    ss << user_data->username << ": " << message;
    return ss.str();
}

int main()
{
    std::cout << "Welcome to the homechat server!\n";

    std::mutex message_cache_mutex;
    std::unique_ptr<MessageCache> message_cache = std::make_unique<MessageCache>();

    /* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support.
     * You may swap to using uWS:App() if you don't need SSL */
    uWS::App({/* There are example certificates in uWebSockets.js repo */
              .key_file_name = "misc/key.pem",
              .cert_file_name = "misc/cert.pem",
              .passphrase = "1234"})
        .ws<PerSocketData>(
            "/*",
            {/* Settings */
             .compression = uWS::CompressOptions(uWS::DEDICATED_COMPRESSOR_4KB | uWS::DEDICATED_DECOMPRESSOR),
             .maxPayloadLength = 100 * 1024 * 1024,
             .idleTimeout = 16,
             .maxBackpressure = 100 * 1024 * 1024,
             .closeOnBackpressureLimit = false,
             .resetIdleTimeoutOnSend = false,
             .sendPingsAutomatically = true,
             /* Handlers */
             .upgrade =
                 [](uWS::HttpResponse<false> *res, uWS::HttpRequest *req, us_socket_context_t *ctx) {
                     std::string_view name = req->getHeader("username");
                     if (name.empty())
                     {
                         std::cout << "No username provided\n";
                         res->writeStatus("401 Unauthorized: add `username` header")->end();
                         return;
                     }

                     res->template upgrade<PerSocketData>(
                         {
                             .created_at = DateTime::now(),
                             .username = std::string(name),
                         },
                         req->getHeader("sec-websocket-key"), req->getHeader("sec-websocket-protocol"),
                         req->getHeader("sec-websocket-extensions"), ctx);
                 },
             .open =
                 [&](uWS::WebSocket<false, true, PerSocketData> *ws) {
                     auto user_data = ws->getUserData();

                     auto desc = user_data->username + " connected at: ";
                     DateTime::outputTime(desc.c_str(), user_data->created_at);
                     if (!ws->subscribe("*", true))
                     {
                         std::cout << "Failed to subscribe to * channel\n";
                         ws->close();
                     }

                     std::lock_guard<std::mutex> lock(message_cache_mutex);
                     auto messages = message_cache->last_ten_messages();
                     ws->send(std::string("Welcome to the homechat server! Here are the last 10 messages: \n"),
                              uWS::OpCode::TEXT, false);
                     for (auto &full_message : messages)
                     {
                         ws->send(full_message, uWS::OpCode::TEXT, false);
                     }
                 },
             .message =
                 [&](uWS::WebSocket<false, true, PerSocketData> *ws, std::string_view message, uWS::OpCode opCode) {
                     /* This is the opposite of what you probably want; compress if message is LARGER than 16 kb
                      * the reason we do the opposite here; compress if SMALLER than 16 kb is to allow for
                      * benchmarking of large message sending without compression */
                     std::cout << "Message received: " << message << std::endl;
                     auto user_data = ws->getUserData();
                     auto full_message_formed = full_message(user_data, message);
                     ws->send(full_message_formed, opCode, false);
                     ws->publish("*", full_message_formed, opCode, false);

                     std::lock_guard<std::mutex> lock(message_cache_mutex);
                     if (!message_cache->add_message(full_message_formed))
                     {
                         std::cerr << "Failed to add message to cache" << std::endl;
                     }
                 },
             .dropped =
                 [](auto * /*ws*/, std::string_view /*message*/, uWS::OpCode /*opCode*/) {
                     /* A message was dropped due to set maxBackpressure and closeOnBackpressureLimit limit */
                 },
             .drain =
                 [](auto * /*ws*/) {
                     /* Check ws->getBufferedAmount() here */
                 },
             .ping =
                 [](auto * /*ws*/, std::string_view) {
                     /* Not implemented yet */
                 },
             .pong =
                 [](auto * /*ws*/, std::string_view) {
                     /* Not implemented yet */
                 },
             .close =
                 [](auto * /*ws*/, int /*code*/, std::string_view /*message*/) {
                     /* You may access ws->getUserData() here */
                     std::cout << "Client disconnected\n";
                 }})
        .listen(9001,
                [](auto *listen_socket) {
                    if (listen_socket)
                    {
                        std::cout << "Listening on port " << 9001 << std::endl;
                    }
                })
        .run();

    return 0;
}
