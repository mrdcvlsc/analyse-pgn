#include "analyse_game.hpp"
#include "logger.hpp"
#include "process.hpp"

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
#include <utility>

namespace fs = std::filesystem;

std::string analyse_game(ChessGame &chess_game, const std::string &chess_engine, const UciOptions &opts) {

    // spawn chess uci engine child process

    asio::io_context ctx;
    asio::readable_pipe pipe_stdout{ctx};
    asio::writable_pipe pipe_stdin{ctx};

    auto uci_process = process::process(ctx.get_executor(), chess_engine, {},
        process::process_stdio{pipe_stdin, pipe_stdout, {}});

    error_code ec;

    asio::streambuf child_stdout_buf;
    std::size_t n = 0;

    // initial uci chess engine output

    n = asio::read_until(pipe_stdout, child_stdout_buf, '\n', ec);
    check_for_error("running chess engine failed: ", uci_process, ec);
    std::cout << "engine: " << readline_child_stdout(child_stdout_buf) << '\n';

    // initialize uci chess engine

    std::string init_uci = "uci\n";
    std::cout << ">>> send: " << init_uci;
    asio::write(pipe_stdin, asio::buffer(init_uci), ec);
    check_for_error("failed to send 'uci' to engine: ", uci_process, ec);

    int uci_tries = 0;
    while (true) {
        n = asio::read_until(pipe_stdout, child_stdout_buf, '\n', ec);
        check_for_error("error while waiting for uciok: ", uci_process, ec);

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
    check_for_error("error on '" + set_ponder_false + "': ", uci_process, ec);

    // set uci transposition hash table size to 850MB

    std::string set_hash_size = "setoption name Hash value " + std::to_string(opts.hash_size) + "\n";
    std::cout << ">>> send: " << set_hash_size;
    asio::write(pipe_stdin, asio::buffer(set_hash_size), ec);
    check_for_error("error on '" + set_hash_size + "': ", uci_process, ec);

    // set uci to use ~75% of cpu threads

    std::string set_thread_use_cnt = "setoption name Threads value " + std::to_string(opts.threads) + "\n";
    std::cout << ">>> send: " << set_thread_use_cnt;
    asio::write(pipe_stdin, asio::buffer(set_thread_use_cnt), ec);
    check_for_error("error '" + set_thread_use_cnt + "': ", uci_process, ec);

    // set uci ready

    std::string set_isready = "isready\n";
    std::cout << ">>> send: " << set_isready;
    asio::write(pipe_stdin, asio::buffer(set_isready), ec);
    check_for_error("unable to send isready: ", uci_process, ec);

    n = asio::read_until(pipe_stdout, child_stdout_buf, '\n', ec);
    check_for_error("error on uci 'isready', unable to receive 'readyok': ", uci_process, ec);
    std::cout << "engine: " << readline_child_stdout(child_stdout_buf) << '\n';

    // start a new game

    std::string set_new_game = "ucinewgame\n";
    std::cout << ">>> send: " << set_new_game;
    asio::write(pipe_stdin, asio::buffer(set_new_game), ec);
    check_for_error("unable to start a new game: ", uci_process, ec);

    // start game analysis

    std::string moves;
    moves.reserve(chess_game.moves.size() * 5 + 30);
    moves.append("position startpos moves\n");

    std::cout << ">>> send: " << moves;
    asio::write(pipe_stdin, asio::buffer(moves), ec);
    check_for_error("error sending starting position: ", uci_process, ec);

    moves.pop_back(); // we're removing the new line after feeding it to the child process

    for (int i = 0; i < chess_game.moves.size(); i++) {
        auto moving_player = static_cast<Piece>(i % 2);
        auto move = chess_game.moves[i];

        // get the best move

        std::string line_last_best_move = "", line_2nd_last_best_move = "";

        if (opts.piece == Piece::Both || opts.piece == moving_player) {
            std::string get_best_move = "go depth " + std::to_string(opts.depth) + "\n";
            auto result = get_ucigo_bestmove(uci_process, get_best_move, pipe_stdin, pipe_stdout,
                child_stdout_buf, opts);

            line_last_best_move = result.first;
            line_2nd_last_best_move = result.second;
        }

        // analyse the player's move

        std::string line_last_player_move = "", line_2nd_last_player_move = "";

        if (opts.piece == Piece::Both || opts.piece == moving_player) {
            std::string analyse_player_move =
                "go depth " + std::to_string(opts.depth) + " searchmoves " + move + "\n";

            auto result = get_ucigo_bestmove(uci_process, analyse_player_move, pipe_stdin, pipe_stdout,
                child_stdout_buf, opts);

            line_last_player_move = result.first;
            line_2nd_last_player_move = result.second;
        }

        std::cout << "**************** best move ****************\n";

        std::cout << "+> " << line_2nd_last_best_move << '\n';
        std::cout << "+> " << line_last_best_move << '\n';

        std::cout << "*************** player move ***************\n";

        std::cout << "+> " << line_2nd_last_player_move << '\n';
        std::cout << "+> " << line_last_player_move << '\n';

        std::cout << "*******************************************\n";

        // send moves to the engine

        moves.push_back(' ');
        moves.append(move);
        moves.push_back('\n');

        std::cout << ">>> send: " << moves;
        asio::write(pipe_stdin, asio::buffer(moves), ec);
        check_for_error("error sending moves: ", uci_process, ec);

        moves.pop_back();
    }

    // request uci chess engine exit

    std::string quit_uci = "quit\n";
    std::cout << ">>> send: " << quit_uci;
    asio::write(pipe_stdin, asio::buffer(quit_uci), ec);
    check_for_error("unable to properly close chess engine with 'quit': ", uci_process, ec);

    uci_process.request_exit();
    auto process_return_int = uci_process.wait();

    std::cout << "uci engine return status " << process_return_int << '\n';

    return {};
}

std::pair<std::string, std::string> get_ucigo_bestmove(process::process &uci_process,
    const std::string &go_command,
    asio::writable_pipe &pipe_stdin,
    asio::readable_pipe &pipe_stdout,
    asio::streambuf &child_stdout_buf,
    const UciOptions &opts) {

    error_code ec;

    std::string line_2nd_last = "";
    std::string line_last = "";

    std::cout << ">>> send: " << go_command;
    asio::write(pipe_stdin, asio::buffer(go_command), ec);
    check_for_error("error starting analysis: ", uci_process, ec);

    int line_outputs = 0;

    while (true) {
        asio::read_until(pipe_stdout, child_stdout_buf, '\n', ec);
        check_for_error("error reading analysis output: ", uci_process, ec);

        std::string line = readline_child_stdout(child_stdout_buf);
        std::cout << "engine: " << line << '\n';

        line_2nd_last = line_last;
        line_last = line;

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