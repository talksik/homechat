#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

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
