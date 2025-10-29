#include "logger.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

#ifndef NDEBUG
#include <cstdlib>
#endif

std::string get_timestamp() {
    auto now = std::chrono::system_clock::now();

    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&now_c);

    std::ostringstream time;
    time << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");

    std::string timestamp = time.str();

    timestamp.erase(std::remove_if(timestamp.begin(), timestamp.end(), isspace), timestamp.end());

    return timestamp;
}