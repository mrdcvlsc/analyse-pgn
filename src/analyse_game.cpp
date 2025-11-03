#include "analyse_game.hpp"
#include "logger.hpp"
#include "process_utils.hpp"

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

#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
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
    auto uci_output = readline_child_stdout(child_stdout_buf);
    DEBUG_LOG("engine: " + uci_output);

    // initialize uci chess engine

    std::cout << "running " << chess_engine << "...\n";

    std::string init_uci = "uci\n";
    DEBUG_LOG(">>> send: " + init_uci);
    asio::write(pipe_stdin, asio::buffer(init_uci), ec);
    check_for_error("failed to send 'uci' to engine: ", uci_process, ec);

    int uci_tries = 0;
    while (true) {
        n = asio::read_until(pipe_stdout, child_stdout_buf, '\n', ec);
        check_for_error("error while waiting for uciok: ", uci_process, ec);

        std::string line = readline_child_stdout(child_stdout_buf);
        DEBUG_LOG("engine: " + line);

        if (line == "uciok") {
            break;
        } else {
            uci_tries++;
        }

        if (uci_tries > NOT_EXPECTED_LINE_OUTPUT_LIMIT) {
            throw std::runtime_error("error unexpected outputs after 'uci' input to child");
        }
    }

    std::cout << "setting uci options...\n";

    // set uci no ponder

    std::string set_ponder_false = "setoption name Ponder value false\n";
    DEBUG_LOG(">>> send: " + set_ponder_false);
    asio::write(pipe_stdin, asio::buffer(set_ponder_false), ec);
    check_for_error("error on '" + set_ponder_false + "': ", uci_process, ec);

    // set uci transposition hash table size to 850MB

    std::string set_hash_size = "setoption name Hash value " + std::to_string(opts.hash_size) + "\n";
    DEBUG_LOG(">>> send: " + set_hash_size);
    asio::write(pipe_stdin, asio::buffer(set_hash_size), ec);
    check_for_error("error on '" + set_hash_size + "': ", uci_process, ec);

    // set uci to use ~75% of cpu threads

    std::string set_thread_use_cnt = "setoption name Threads value " + std::to_string(opts.threads) + "\n";
    DEBUG_LOG(">>> send: " + set_thread_use_cnt);
    asio::write(pipe_stdin, asio::buffer(set_thread_use_cnt), ec);
    check_for_error("error '" + set_thread_use_cnt + "': ", uci_process, ec);

    // set uci ready

    std::string set_isready = "isready\n";
    DEBUG_LOG(">>> send: " + set_isready);
    asio::write(pipe_stdin, asio::buffer(set_isready), ec);
    check_for_error("unable to send isready: ", uci_process, ec);

    n = asio::read_until(pipe_stdout, child_stdout_buf, '\n', ec);
    check_for_error("error on uci 'isready', unable to receive 'readyok': ", uci_process, ec);
    auto uci_isready_output = readline_child_stdout(child_stdout_buf);
    DEBUG_LOG("engine: " + uci_isready_output);

    // start a new game

    std::string set_new_game = "ucinewgame\n";
    DEBUG_LOG(">>> send: " + set_new_game);
    asio::write(pipe_stdin, asio::buffer(set_new_game), ec);
    check_for_error("unable to start a new game: ", uci_process, ec);

    std::cout << "initial setups complete\n";

    // start game analysis

    std::string moves, bestmove_keyword = "bestmove ";
    moves.reserve(chess_game.moves.size() * 5 + 30);
    moves.append("position startpos moves\n");

    DEBUG_LOG(">>> send: " + moves);
    asio::write(pipe_stdin, asio::buffer(moves), ec);
    check_for_error("error sending starting position: ", uci_process, ec);

    moves.pop_back(); // we're removing the new line after feeding it to the child process

    std::cout << "analyzing current game...\n";

    for (int i = 0; i < (chess_game.moves.size() - 1) && i < opts.until_move; i++) {
        DEBUG_LOG("turn : " + std::to_string(i + 1) + "\n");
        DEBUG_LOG("moves: " + std::to_string(chess_game.moves.size()) + "\n");

        auto moving_player = static_cast<Piece>(i % 2);
        auto move          = chess_game.moves[i];

        DEBUG_LOG("move: " + move + "\n");

        if (i >= opts.start_on_move * 2) {
            // get the best move

            std::string move_best, bestmove_continuation = "";
            int move_best_cp = 0; // best move's centipawn
            int move_best_cm = 0; // best move's checkmate after certain 'n' moves.

            if (opts.piece == Piece::Both || opts.piece == moving_player) {
                std::string get_best_move = "go depth " + std::to_string(opts.depth) + "\n";

                auto result = get_ucigo_bestmove(uci_process, get_best_move, pipe_stdin, pipe_stdout,
                    child_stdout_buf, opts);

                auto bm_idx = result.first.find(bestmove_keyword);
                if (bm_idx != std::string::npos) {
                    move_best = result.first.substr(bm_idx + bestmove_keyword.size(), 4);
                }

                auto pv_keyword = std::string("pv "); // principal variation uci chess engine go result keyword
                auto bmc_pv_idx = result.second.find(pv_keyword);
                if (bmc_pv_idx != std::string::npos) {
                    auto bestmove_idx = result.second.find(move_best);
                    if (bestmove_idx != std::string::npos) {
                        bestmove_continuation = result.second.substr(bestmove_idx + move_best.size());
                    }

                    if (bestmove_continuation[0] == ' ') {
                        bestmove_continuation = bestmove_continuation.substr(1);
                    }
                }

                std::string item;
                std::istringstream strstream(result.second);

                while (strstream >> item) {
                    if (item == "cp") {
                        strstream >> item;
                        move_best_cp = std::atoi(item.c_str());
                        break;
                    }

                    if (item == "mate") {
                        strstream >> item;
                        move_best_cm = std::atoi(item.c_str());
                        break;
                    }
                }
            }

            // analyse the player's move

            std::string move_player;
            int move_player_cp = 0; // best move's centipawn
            int move_player_cm = 0; // best move's checkmate after certain 'n' moves.

            if (opts.piece == Piece::Both || opts.piece == moving_player) {
                if (move == move_best) {
                    move_player            = move_best;
                    move_player_cp         = move_best_cp;
                    move_player_cm         = move_best_cm;
                    chess_game.comments[i] = " {" + score_comments(0, move_best_cp);
                } else {
                    std::string analyse_player_move =
                        "go depth " + std::to_string(opts.depth) + " searchmoves " + move + "\n";

                    auto result = get_ucigo_bestmove(uci_process, analyse_player_move, pipe_stdin, pipe_stdout,
                        child_stdout_buf, opts);

                    auto bm_idx = result.first.find(bestmove_keyword);
                    if (bm_idx != std::string::npos) {
                        move_player = result.first.substr(bm_idx + bestmove_keyword.size(), 4);
                    }

                    std::string item;
                    std::istringstream strstream(result.second);

                    while (strstream >> item) {
                        if (item == "cp") {
                            strstream >> item;
                            move_player_cp = std::atoi(item.c_str());
                            break;
                        }

                        if (item == "mate") {
                            strstream >> item;
                            move_player_cm = std::atoi(item.c_str());
                            break;
                        }
                    }

                    int to_best_move_diff = move_player_cp - move_best_cp;
                    chess_game.comments[i] += " {" + score_comments(to_best_move_diff, move_player_cp);
                }

                chess_game.comments[i] += (!move_player_cp && move_player_cm)
                                              ? (" MATE_IN:" + std::to_string(move_player_cm))
                                              : (" CP:" + std::to_string(move_player_cp));

                chess_game.comments[i] += " } ";

                if (move != move_best) {
                    chess_game.comments[i] +=
                        " (" + move_best + " { was the best move" +
                        ((!move_best_cp && move_best_cm) ? (" MATE_IN:" + std::to_string(move_best_cm))
                                                         : (" CP:" + std::to_string(move_best_cp))) +
                        (", followed by } " + bestmove_continuation + " ) ");
                }
            }

            DEBUG_LOG("**************** best move ****************");

            DEBUG_LOG("+> best move " + move_best +
                      ((!move_best_cp && move_best_cm) ? (" checkmate in " + std::to_string(move_best_cm))
                                                       : (" centipawn " + std::to_string(move_best_cp))));

            DEBUG_LOG("*************** player move ***************");

            DEBUG_LOG("+> player move " + move_player +
                      ((!move_player_cp && move_player_cm) ? (" checkmate in " + std::to_string(move_player_cm))
                                                           : (" centipawn " + std::to_string(move_player_cp))));

            DEBUG_LOG("*******************************************");
        }

        // send moves to the engine

        moves.push_back(' ');
        moves.append(move);
        moves.push_back('\n');

        DEBUG_LOG(">>> send: " + moves);
        asio::write(pipe_stdin, asio::buffer(moves), ec);
        check_for_error("error sending moves: ", uci_process, ec);

        moves.pop_back();
    }

    std::cout << "analysis done, killing " << chess_engine << " child process now...\n";

    // request uci chess engine exit

    std::string quit_uci = "quit\n";
    DEBUG_LOG(">>> send: " + quit_uci);
    asio::write(pipe_stdin, asio::buffer(quit_uci), ec);
    check_for_error("unable to properly close chess engine with 'quit': ", uci_process, ec);

    uci_process.request_exit();
    auto process_return_int = uci_process.wait();

    if (process_return_int) {
        std::cout << "uci chess engine return status : " << process_return_int << '\n';
        std::cout << "warning: an error might have occured\n";
    } else {
        std::cout << "uci engine return status : " << process_return_int << " <- no errors : success";
    }

    std::cout << "adding comments...\n";

    std::string analyzed_game;
    analyzed_game.reserve(
        chess_game.moves.size() * 5 + chess_game.tags.size() * 15 + chess_game.comments.size() * 30);

    for (const auto &tag : chess_game.tags) {
        auto key   = tag.first;
        auto value = tag.second;
        analyzed_game.append("[ ");
        analyzed_game.append(key);
        analyzed_game.append(" \"");
        analyzed_game.append(value);
        analyzed_game.append("\" ]\n");
    }

    analyzed_game.push_back('\n');

    for (int i = 0; i < chess_game.moves.size(); i++) {
        auto moving_player = static_cast<Piece>(i % 2);

        analyzed_game.append(chess_game.moves[i]);

        if (opts.piece == Piece::Both || opts.piece == moving_player) {
            if (chess_game.comments.find(i) != chess_game.comments.end()) {
                analyzed_game.append(chess_game.comments[i]);
            }
        } else {
            analyzed_game.push_back(' ');
        }
    }

    analyzed_game.push_back('\n');

    return analyzed_game;
}

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

std::string score_comments(int centipawn_difference, int player_move_centipawn) {
    std::string comment = "";
    if (centipawn_difference == 0) {
        if (player_move_centipawn > 0) {
            comment = " brilliant !!!";
        } else {
            comment = " accurate !!!";
        }
    } else if (centipawn_difference > -10) {
        if (player_move_centipawn > 0) {
            comment = " excellent !!";
        } else {
            comment = " accurate !!";
        }
    } else if (centipawn_difference > -30) {
        if (player_move_centipawn > 0) {
            comment = " good !";
        } else {
            comment = " questionable ?";
        }
    } else if (centipawn_difference > -60) {
        comment = " inaccurate ?";
    } else if (centipawn_difference > -140) {
        comment = " mistake ??";
    } else {
        if (player_move_centipawn < 0) {
            comment = " blunder ???";
        } else {
            comment = " missed ???";
        }
    }

    if (player_move_centipawn > 150) {
        comment += " WINNING";
    } else if (player_move_centipawn < -150) {
        comment += " LOSING";
    } else if (player_move_centipawn > 115) {
        comment += " ADVANTAGE";
    } else if (player_move_centipawn < -115) {
        comment += " DISDVANTAGE";
    } else {
        comment += " UNCERTAIN";
    }

    return comment;
}