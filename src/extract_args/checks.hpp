#pragma once

#include <string>

bool is_number(const std::string &input);

void assert_missing_flagvalue(int index, std::size_t arg_cnt, const std::string &argument);

void assert_invalid(const std::string &what, const std::string &param_flag, const std::string &value);