#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#endif

#include "boost/process/process.hpp"
#include "boost/system/detail/error_code.hpp"
#include "chess_games.hpp"

#include <string>

std::string analyse_game(const ChessGame &chess_game, const std::string &chess_engine);

void interpret_error(const std::string &msg,
    boost::process::process &child_process,
    const boost::system::error_code &ec);