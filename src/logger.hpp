#pragma once

#include <ctime>
#include <string>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <thread>

std::string get_timestamp();

#ifndef NDEBUG
    #define DEBUG_LOG(x) std::cout << get_timestamp() << "|t(" << std::this_thread::get_id() << ")" << "[" << std::filesystem::path(std::string(__FILE__)).filename().string()  << ':' << __LINE__ << "]: " << (x) << '\n';
    #define DEBUG_ERR(x) std::cerr << get_timestamp() << "|t(" << std::this_thread::get_id() << ")" << "[" << std::filesystem::path(std::string(__FILE__)).filename().string()  << ':' << __LINE__ << "]: " << (x) << '\n';
    #define DEBUG_LOG_HEX(x) std::cout << get_timestamp() << "|t(" << std::this_thread::get_id() << ")" << "[" << std::filesystem::path(std::string(__FILE__)).filename().string()  << ':' << __LINE__ << "]: " << std::hex << (x) << std::dec << '\n';
    #define DEBUG_ERR_HEX(x) std::cerr << get_timestamp() << "|t(" << std::this_thread::get_id() << ")" << "[" << std::filesystem::path(std::string(__FILE__)).filename().string()  << ':' << __LINE__ << "]: " << std::hex << (x) << std::dec << '\n';
#else
    #define DEBUG_LOG(x) // noop
    #define DEBUG_ERR(x) // noop
    #define DEBUG_LOG_HEX(x) // noop
    #define DEBUG_ERR_HEX(x) // noop
#endif
