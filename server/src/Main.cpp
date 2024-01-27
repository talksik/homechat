#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <uWebSockets/App.h>
#include <uWebSockets/WebSocket.h>

namespace DateTime
{
std::chrono::system_clock::time_point now()
{
    return std::chrono::system_clock::now();
}

void outputTime(const char *desc, std::chrono::system_clock::time_point &tp)
{
    std::time_t now = std::chrono::system_clock::to_time_t(tp);
    std::cout << desc << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M") << "\n";
}
} // namespace DateTime

int main()
{
    std::cout << "Welcome to the homechat server!\n";

    /* ws->getUserData returns one of these */
    struct PerSocketData
    {
        std::chrono::system_clock::time_point created_at;
    };

    /* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support.
     * You may swap to using uWS:App() if you don't need SSL */
    uWS::App({/* There are example certificates in uWebSockets.js repo */
              .key_file_name = "misc/key.pem",
              .cert_file_name = "misc/cert.pem",
              .passphrase = "1234"})
        .ws<PerSocketData>(
            "/*", {/* Settings */
                   .compression = uWS::CompressOptions(uWS::DEDICATED_COMPRESSOR_4KB | uWS::DEDICATED_DECOMPRESSOR),
                   .maxPayloadLength = 100 * 1024 * 1024,
                   .idleTimeout = 16,
                   .maxBackpressure = 100 * 1024 * 1024,
                   .closeOnBackpressureLimit = false,
                   .resetIdleTimeoutOnSend = false,
                   .sendPingsAutomatically = true,
                   /* Handlers */
                   .upgrade = nullptr,
                   .open =
                       [](uWS::WebSocket<false, true, PerSocketData> *ws) {
                           auto user_data = ws->getUserData();
                           user_data->created_at = DateTime::now();

                           DateTime::outputTime("Client connected at ", user_data->created_at);
                       },
                   .message =
                       [](auto *ws, std::string_view message, uWS::OpCode opCode) {
                           /* This is the opposite of what you probably want; compress if message is LARGER than 16 kb
                            * the reason we do the opposite here; compress if SMALLER than 16 kb is to allow for
                            * benchmarking of large message sending without compression */
                           std::cout << "Message received: " << message << std::endl;
                           ws->send(message, opCode, message.length() < 16 * 1024);
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
