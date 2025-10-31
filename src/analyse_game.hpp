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

void check_for_error(const std::string &msg, process::process &child_process, const error_code &ec);
std::string readline_child_stdout(asio::streambuf &child_stdout_buf);
std::string analyse_game(const ChessGame &chess_game, const std::string &chess_engine);
