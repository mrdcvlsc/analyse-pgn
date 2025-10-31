#include "analyse_game.hpp"
#include "logger.hpp"

#include <cstddef>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <thread>

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/impl/read.hpp>
#include <boost/asio/impl/read_until.hpp>
#include <boost/asio/impl/write.hpp>
#include <boost/asio/readable_pipe.hpp>
#include <boost/asio/registered_buffer.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/writable_pipe.hpp>
#include <boost/process.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/system/detail/error_code.hpp>

namespace fs = std::filesystem;

const int NOT_EXPECTED_LINE_OUTPUT_LIMIT = 1'000;

void check_for_error(const std::string &msg, process::process &child_process, const error_code &ec) {
    if (ec && ec != asio::error::eof) {
        child_process.request_exit();
        child_process.wait();
        throw std::runtime_error(std::string("running chess engine failed: ") + ec.message());
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

std::string analyse_game(const ChessGame &chess_game, const std::string &chess_engine) {

    // spawn chess uci engine child process

    asio::io_context ctx;
    asio::readable_pipe pipe_stdout{ctx};
    asio::writable_pipe pipe_stdin{ctx};

    auto child_process = process::process(ctx.get_executor(), chess_engine, {},
        process::process_stdio{pipe_stdin, pipe_stdout, {}});

    error_code ec;

    asio::streambuf child_stdout_buf;
    std::size_t n = 0;

    // initial uci chess engine output

    n = asio::read_until(pipe_stdout, child_stdout_buf, '\n', ec);
    check_for_error("running chess engine failed: ", child_process, ec);
    std::cout << "engine: " << readline_child_stdout(child_stdout_buf) << '\n';

    // initialize uci chess engine

    std::string init_uci = "uci\n";
    std::cout << ">>> send: " << init_uci;
    asio::write(pipe_stdin, asio::buffer(init_uci), ec);
    check_for_error("failed to send 'uci' to engine: ", child_process, ec);

    int uci_tries = 0;
    while (true) {
        n = asio::read_until(pipe_stdout, child_stdout_buf, '\n', ec);
        check_for_error("error while waiting for uciok: ", child_process, ec);

        std::string line = readline_child_stdout(child_stdout_buf);
        std::cout << "engine: " << line << '\n';

        if (line == "uciok") {
            break;
        } else {
            uci_tries++;
        }

        if (uci_tries > NOT_EXPECTED_LINE_OUTPUT_LIMIT) {
            throw std::runtime_error("error unexpected outputs after 'uci' input to child");
        }
    }

    // set uci no ponder

    std::string set_ponder_false = "setoption name Ponder value false\n";
    std::cout << ">>> send: " << set_ponder_false;
    asio::write(pipe_stdin, asio::buffer(set_ponder_false), ec);
    check_for_error("error on '" + set_ponder_false + "': ", child_process, ec);

    // set uci transposition hash table size to 850MB

    std::string set_hash_size = "setoption name Hash value 850\n";
    std::cout << ">>> send: " << set_hash_size;
    asio::write(pipe_stdin, asio::buffer(set_hash_size), ec);
    check_for_error("error on '" + set_hash_size + "': ", child_process, ec);

    // set uci to use ~75% of cpu threads

    auto thread_cnt = std::thread::hardware_concurrency();
    auto threads_to_use = static_cast<int>(static_cast<float>(thread_cnt) * 0.75f);

    if (threads_to_use <= 0 || threads_to_use > 512) {
        threads_to_use = 1;
    }

    DEBUG_LOG("Threads to use by the chess engine : " + std::to_string(threads_to_use));

    std::string set_thread_use_cnt = "setoption name Threads value " + std::to_string(threads_to_use) + "\n";
    std::cout << ">>> send: " << set_thread_use_cnt;
    asio::write(pipe_stdin, asio::buffer(set_thread_use_cnt), ec);
    check_for_error("error '" + set_thread_use_cnt + "': ", child_process, ec);

    // set uci ready

    std::string set_isready = "isready\n";
    std::cout << ">>> send: " << set_isready;
    asio::write(pipe_stdin, asio::buffer(set_isready), ec);
    check_for_error("unable to send isready: ", child_process, ec);

    n = asio::read_until(pipe_stdout, child_stdout_buf, '\n', ec);
    check_for_error("error on uci 'isready', unable to receive 'readyok': ", child_process, ec);
    std::cout << "engine: " << readline_child_stdout(child_stdout_buf) << '\n';

    // request uci chess engine exit

    std::string quit_uci = "quit\n";
    std::cout << ">>> send: " << quit_uci;
    asio::write(pipe_stdin, asio::buffer(quit_uci), ec);
    check_for_error("unable to properly close chess engine with 'quit': ", child_process, ec);

    child_process.request_exit();
    auto process_return_int = child_process.wait();

    std::cout << "uci engine return status " << process_return_int << '\n';

    return {};
}
