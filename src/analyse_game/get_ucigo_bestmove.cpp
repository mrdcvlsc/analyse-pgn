#include "get_ucigo_bestmove.hpp"
#include "../utils/logger.hpp"

std::pair<std::string, std::string> get_ucigo_bestmove(process::process &uci_process,
    const std::string &go_command,
    asio::writable_pipe &pipe_stdin,
    asio::readable_pipe &pipe_stdout,
    asio::streambuf &child_stdout_buf,
    const UciOptions &opts) {

    error_code ec;

    std::string line_2nd_last = "";
    std::string line_last     = "";

    DEBUG_LOG(">>> send: " + go_command);
    asio::write(pipe_stdin, asio::buffer(go_command), ec);
    check_for_error("error starting analysis: ", uci_process, ec);

    int line_outputs = 0;

    while (true) {
        asio::read_until(pipe_stdout, child_stdout_buf, '\n', ec);
        check_for_error("error reading analysis output: ", uci_process, ec);

        std::string line = readline_child_stdout(child_stdout_buf);
        DEBUG_LOG("engine: " + line);

        line_2nd_last = line_last;
        line_last     = line;

        if (line.find("bestmove") != std::string::npos) {
            break;
        } else {
            line_outputs++;
        }

        if (line_outputs > NOT_EXPECTED_LINE_OUTPUT_LIMIT) {
            throw std::runtime_error("error unexpected outputs after 'uci' input to child");
        }
    }

    return std::make_pair(line_last, line_2nd_last);
}