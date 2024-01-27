#include "include/colors.h"
#include <iostream>

int main()
{
    std::cout << "Please enter your first name: ";
    char *name = new char[100];
    std::cin >> name;
    std::string welcome_message = "Welcome, " + std::string(name) + "!";
    std::cout << std::endl << BOLDGREEN << welcome_message << RESET << std::endl;

    return 0;
}
