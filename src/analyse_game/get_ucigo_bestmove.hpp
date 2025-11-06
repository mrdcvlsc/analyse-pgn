#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#endif

#include "analyse_game.hpp"

#include <boost/asio.hpp>
#include <boost/process.hpp>


#include <string>
#include <utility>

namespace process = boost::process;
namespace asio    = boost::asio;

std::pair<std::string, std::string> get_ucigo_bestmove(process::process &uci_process,
    const std::string &go_command,
    asio::writable_pipe &pipe_stdin,
    asio::readable_pipe &pipe_stdout,
    asio::streambuf &child_stdout_buf,
    const UciOptions &opts);