#pragma once

#include "analyse_game.hpp"
#include "logger.hpp"

#include <filesystem>
#include <utility>
#include <vector>

struct ArgPaths {
    std::string chess_engine_exe;
    std::vector<std::filesystem::path> chess_pgn_files;
};

bool is_number(const std::string &input);

std::pair<std::string, UciOptions> get_defaults();

std::pair<ArgPaths, UciOptions> extract_args(int argc, char *argv[]);

void assert_missing_flagvalue(int index, std::size_t arg_cnt, const std::string &argument);

void assert_invalid(const std::string &what, const std::string &param_flag, const std::string &value);