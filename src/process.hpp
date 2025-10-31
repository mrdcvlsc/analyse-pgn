#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#endif

#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <boost/system/detail/error_code.hpp>

#include <string>

namespace process = boost::process;
namespace asio = boost::asio;
using error_code = boost::system::error_code;

void check_for_error(const std::string &msg, process::process &child_process, const error_code &ec);
std::string readline_child_stdout(asio::streambuf &child_stdout_buf);