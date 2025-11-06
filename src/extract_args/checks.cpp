#include "checks.hpp"

#include <iostream>

bool is_number(const std::string &input) {
    std::string::const_iterator i = input.begin();
    while (i != input.end() && std::isdigit(*i)) {
        ++i;
    }

    return !input.empty() && i == input.end();
}

void assert_missing_flagvalue(int index, std::size_t arg_cnt, const std::string &argument) {
    if (index + 1 >= arg_cnt) {
        std::cerr << "INVALID ARGUMENT:\n\n\tThe given flag" << argument << "' does not have a value\n\n";
        exit(1);
    }
}

void assert_invalid(const std::string &what, const std::string &param_flag, const std::string &value) {
    std::cerr << "Argument Error: " << param_flag << " " << value << "\n\n\tDetected invalid " << what
              << " given\n\tin the parameter flag " << param_flag << "\n";
    exit(1);
}