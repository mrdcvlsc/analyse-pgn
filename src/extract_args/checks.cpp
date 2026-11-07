#include "checks.hpp"

#include <iostream>

bool is_number(const std::string &input) {
    std::string::const_iterator i = input.begin();
    while (i != input.end() && std::isdigit(*i)) {
        ++i;
    }

    return !input.empty() && i == input.end();
}

bool is_pawn_number(const std::string &input) {
    const auto dot = input.find('.');

    if (dot == std::string::npos) {
        return is_number(input);
    }

    const auto whole    = input.substr(0, dot);
    const auto fraction = input.substr(dot + 1);

    // is_number rejects an empty part, and a second '.' would land inside `fraction`
    return is_number(whole) && is_number(fraction) && fraction.size() <= 2;
}

void assert_missing_flagvalue(int index, std::size_t arg_cnt, const std::string &argument) {
    if (index + 1 >= arg_cnt) {
        std::cerr << "INVALID ARGUMENT:\n\n\tThe given flag '" << argument << "' does not have a value\n\n";
        exit(1);
    }
}

void assert_invalid(const std::string &what, const std::string &param_flag, const std::string &value) {
    std::cerr << "Argument Error: " << param_flag << " " << value << "\n\n\tDetected invalid " << what
              << " given\n\tin the parameter flag " << param_flag << "\n";
    exit(1);
}