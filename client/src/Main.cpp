#include "include/colors.h"
#include "include/easywsclient.hpp"
#include <assert.h>
#include <atomic>
#include <iostream>
#include <thread>

static easywsclient::WebSocket::pointer ws = NULL;

void handle_message(const std::string &message)
{
    printf(">>> %s\n", message.c_str());
}

int main()
{
    std::cout << "Please enter your first name: ";
    char *name = new char[100];
    std::cin >> name;
    std::string welcome_message = "Welcome, " + std::string(name) + "!";
    std::cout << std::endl << BOLDGREEN << welcome_message << RESET << std::endl;
    std::cout << CYAN << "Connecting you to the server..." << RESET << std::endl;

    ws = easywsclient::WebSocket::from_url("ws://localhost:9001", "", name);
    assert(ws);

    std::cout << GREEN << "Connected! Type q, quit, or exit to end session." << RESET << std::endl;
    std::cout << BOLDRED << std::endl;

    auto receiver = std::thread([&] {
        std::string s;
        while (ws->getReadyState() != easywsclient::WebSocket::CLOSED)
        {
            ws->poll(-1);
            ws->dispatch(handle_message);
        }
    });

    std::cout << BOLDGREEN << "Enter message to send (q, quit, or exit to end session): " << RESET << BOLDRED
              << std::endl;
    while (true)
    {
        std::string input;
        std::getline(std::cin, input);

        if (input == "quit" || input == "exit" || input == "q")
        {
            std::cout << "Exiting the program. Goodbye!\n";
            break;
        }

        std::cout << "Sending message: " << input << std::endl;

        ws->send(input);
    }

    receiver.join();
    ws->close();

    delete[] name;
    delete ws;

    return 0;
}
