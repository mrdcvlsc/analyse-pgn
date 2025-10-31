#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#endif

#include "chess_games.hpp"

#include <boost/asio/streambuf.hpp>
#include <boost/process/process.hpp>
#include <boost/system/detail/error_code.hpp>

#include <string>

using error_code = boost::system::error_code;
namespace process = boost::process;
namespace asio = boost::asio;


std::string analyse_game(const ChessGame &chess_game, const std::string &chess_engine);
