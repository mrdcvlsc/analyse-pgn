#include "process_utils.hpp"

#include <boost/asio/error.hpp>

#include <iostream>
#include <stdexcept>

void check_for_error(const std::string &msg, process::process &child_process, const error_code &ec) {
    if (ec) {
        // end-of-file means the child process closed its output (it exited or crashed), that is an error too.
        // a broken pipe means the same: windows reports a closed pipe that way instead of end-of-file, and
        // writing to a child process that has already exited fails with it on every platform.
        std::string what = msg + ec.message();
        if (ec == asio::error::eof || ec == asio::error::broken_pipe) {
            what += " (the program stopped responding, it may have exited or crashed)";
        }

        child_process.request_exit();
        throw std::runtime_error(what);
    }
}

std::string readline_child_stdout(asio::streambuf &child_stdout_buf) {
    std::istream read_istream(&child_stdout_buf);
    std::string child_output_line;
    std::getline(read_istream, child_output_line);

    if (!child_output_line.empty() && child_output_line.back() == '\r') {
        child_output_line.pop_back();
    }

    return child_output_line;
}