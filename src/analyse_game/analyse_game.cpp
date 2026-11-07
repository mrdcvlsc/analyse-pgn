#include "analyse_game.hpp"
#include "get_ucigo_bestmove.hpp"
#include "score_comments.hpp"

#include "../utils/logger.hpp"
#include "../utils/process_utils.hpp"

#include <algorithm>
#include <array>
#include <cctype>
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

#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <ios>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

namespace fs = std::filesystem;

// returns the move after "bestmove" in a uci `bestmove <move> [ponder <move>]` line.
// moves are 4 characters long, or 5 for promotions (e.g. "e7e8q"), so they must not be cut to a fixed length.
static std::string parse_bestmove(const std::string &bestmove_line) {
    std::istringstream words(bestmove_line);
    std::string word;

    while (words >> word) {
        if (word == "bestmove" && words >> word) {
            return word;
        }
    }

    return "";
}

// a centipawn score in pawns: "0.25", "-1.30", or with plus_sign "+2.14"
static std::string pawns_text(int centipawns, bool plus_sign) {
    const auto absolute  = std::abs(centipawns);
    const auto whole     = absolute / 100;
    const auto remainder = absolute % 100;
    const auto sign      = (centipawns < 0) ? "-" : (plus_sign ? "+" : "");

    return sign + std::to_string(whole) + "." + ((remainder < 10) ? "0" : "") + std::to_string(remainder);
}

// a standard [%eval] tag for the position after the move, as lichess writes it: pawns from white's point of
// view ("[%eval 0.25]"), or "#n" for a forced mate in n moves ("[%eval #3]", negative when black mates).
// apgn's scores are the engine's, for the moving player and counting the move itself: they are flipped for
// black's moves, and a mate the moving player gives has one move fewer left after the move. a move that
// delivers mate gets no tag, since the game is over.
static std::string eval_tag(int centipawns, int mate_in_moves, bool white_to_move) {
    const int white_view = white_to_move ? 1 : -1;

    if (!centipawns && mate_in_moves) {
        // a mate against the moving player is still that many moves away after the move
        const int moves_left = (mate_in_moves > 0) ? mate_in_moves - 1 : mate_in_moves;

        if (moves_left == 0) {
            return "";
        }

        return " [%eval #" + std::to_string(moves_left * white_view) + "]";
    }

    return " [%eval " + pawns_text(centipawns * white_view, false) + "]";
}

// apgn's own score text, from the moving player's point of view: " CP:25", or " MATE_IN:3" for a forced mate
static std::string score_text(int centipawns, int mate_in_moves) {
    return (!centipawns && mate_in_moves) ? (" MATE_IN:" + std::to_string(mate_in_moves))
                                          : (" CP:" + std::to_string(centipawns));
}

// pgn-extract writes promotions as "b7a8Q", uci engines use (and answer with) a lowercase piece: "b7a8q".
static std::string to_uci_move(std::string move) {
    if (move.size() == 5) {
        move[4] = static_cast<char>(std::tolower(static_cast<unsigned char>(move[4])));
    }

    return move;
}

std::string analyse_game(ChessGame &chess_game, const std::string &chess_engine, const UciOptions &opts) {

    // spawn chess uci engine child process

    asio::io_context ctx;
    asio::readable_pipe pipe_stdout{ctx};
    asio::writable_pipe pipe_stdin{ctx};

    auto uci_process = process::process(ctx.get_executor(), chess_engine, {},
        process::process_stdio{pipe_stdin, pipe_stdout, {}});

    error_code ec;

    asio::streambuf child_stdout_buf;

    // initialize uci chess engine. an engine may print a banner when it starts (stockfish does) or print
    // nothing until its first command (uci does not require a banner), so apgn does not wait for one: any
    // banner is read and skipped by the wait for "uciok" below.

    std::cout << "running " << chess_engine << "...\n";

    std::string init_uci = "uci\n";
    DEBUG_LOG(">>> send: " + init_uci);
    asio::write(pipe_stdin, asio::buffer(init_uci), ec);
    check_for_error("failed to send 'uci' to engine: ", uci_process, ec);

    int uci_tries = 0;
    while (true) {
        asio::read_until(pipe_stdout, child_stdout_buf, '\n', ec);
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

    asio::read_until(pipe_stdout, child_stdout_buf, '\n', ec);
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

    // a game can start from a custom position (SetUp/FEN tags), possibly with black to move first

    std::string start_position = "startpos";
    for (const auto &[tag, value] : chess_game.tags) {
        if (tag == "FEN") {
            start_position = "fen " + value;

            std::istringstream fen_fields(value);
            std::string piece_placement, side_to_move, castling, en_passant;
            int halfmove_clock = 0, fullmove_number = 0;
            fen_fields >> piece_placement >> side_to_move >> castling >> en_passant >> halfmove_clock >> fullmove_number;
            chess_game.first_player = (side_to_move == "b") ? 1 : 0;

            // a FEN without a usable move number counts from 1
            chess_game.first_move_number = (fullmove_number > 0) ? fullmove_number : 1;
        }
    }

    std::string moves;
    moves.reserve(chess_game.moves.size() * 5 + start_position.size() + 30);
    moves.append("position " + start_position + " moves\n");

    DEBUG_LOG(">>> send: " + moves);
    asio::write(pipe_stdin, asio::buffer(moves), ec);
    check_for_error("error sending starting position: ", uci_process, ec);

    moves.pop_back(); // we're removing the new line after feeding it to the child process

    std::cout << "analyzing current game...\n";
    auto start = std::chrono::high_resolution_clock::now();

    std::array<int, 2> prev_cm_in_nmoves{0, 0};

    // until_move is a move number; each move is two plies (white + black)
    for (int i = 0; i < (chess_game.moves.size() - 1) && i < opts.until_move * 2; i++) {
        std::cout << "\r" << static_cast<float>(i) / static_cast<float>(chess_game.moves.size() - 1) * 100.f
                  << "%...";

        DEBUG_LOG("turn : " + std::to_string(i + 1) + "\n");
        DEBUG_LOG("moves: " + std::to_string(chess_game.moves.size()) + "\n");

        const auto moving_player = static_cast<Piece>((i + chess_game.first_player) % 2);
        const auto curr_player   = (i + chess_game.first_player) % 2;
        const auto move          = to_uci_move(chess_game.moves[i]);

        DEBUG_LOG("move: " + move + "\n");

        // start_on_move is a 1-based move number (0 and 1 both mean "from the first move")
        if (i >= std::max(0, opts.start_on_move - 1) * 2) {
            // get the best move

            std::string mv_best, best_mv_cont = "";
            int mv_best_cp = 0; // best move's centipawn
            int mv_best_cm = 0; // best move's checkmate after certain 'n' moves.

            if (opts.piece == Piece::Both || opts.piece == moving_player) {
                std::string get_best_mv = "go depth " + std::to_string(opts.depth) + "\n";

                auto result =
                    get_ucigo_bestmove(uci_process, get_best_mv, pipe_stdin, pipe_stdout, child_stdout_buf, opts);

                mv_best = parse_bestmove(result.first);

                auto pv_keyword = std::string("pv "); // principal variation uci chess engine go result keyword
                auto bmc_pv_idx = result.second.find(pv_keyword);
                if (bmc_pv_idx != std::string::npos) {
                    auto bestmove_idx = result.second.find(mv_best);
                    if (bestmove_idx != std::string::npos) {
                        best_mv_cont = result.second.substr(bestmove_idx + mv_best.size());
                    }

                    if (best_mv_cont[0] == ' ') {
                        best_mv_cont = best_mv_cont.substr(1);
                    }
                }

                std::string item;
                std::istringstream strstream(result.second);

                while (strstream >> item) {
                    if (item == "cp") {
                        strstream >> item;
                        mv_best_cp = std::atoi(item.c_str());
                        break;
                    }

                    if (item == "mate") {
                        strstream >> item;
                        mv_best_cm = std::atoi(item.c_str());
                        break;
                    }
                }
            }

            // analyse the player's move

            std::string mv_player, comment = "", comment_type = "", tmp_item;
            int mv_player_cp = 0; // best move's centipawn
            int mv_player_cm = 0; // best move's checkmate after certain 'n' moves.

            if (opts.piece == Piece::Both || opts.piece == moving_player) {
                if (move == mv_best) {
                    mv_player    = mv_best;
                    mv_player_cp = mv_best_cp;
                    mv_player_cm = mv_best_cm;
                } else {
                    std::string analyse_player_mv =
                        "go depth " + std::to_string(opts.depth) + " searchmoves " + move + "\n";

                    auto result = get_ucigo_bestmove(uci_process, analyse_player_mv, pipe_stdin, pipe_stdout,
                        child_stdout_buf, opts);

                    mv_player = parse_bestmove(result.first);

                    std::string item;
                    std::istringstream strstream(result.second);

                    while (strstream >> item) {
                        if (item == "cp") {
                            strstream >> item;
                            mv_player_cp = std::atoi(item.c_str());
                            break;
                        }

                        if (item == "mate") {
                            strstream >> item;
                            mv_player_cm = std::atoi(item.c_str());
                            break;
                        }
                    }
                }

                int cp_pmv_bmv_diff = mv_player_cp - mv_best_cp;
                comment = score_comments(cp_pmv_bmv_diff, mv_player_cp, mv_best_cp, mv_player_cm, mv_best_cm);

                // the whole annotation of this move is built in this one string, and written once below
                // unless --min-loss leaves the move unannotated
                std::string move_comment;

                std::istringstream interpret_stream(comment);
                interpret_stream >> tmp_item;
                comment_type.append(tmp_item);
                comment_type.push_back(' ');
                interpret_stream >> tmp_item;
                comment_type.append(tmp_item);

                // score_comments() returns " <label> <status>", e.g. " blunder !?? LOSING"
                std::string position_status;
                interpret_stream >> position_status;

                if (chess_game.interpret_stats.at(curr_player).find(comments::rank(comment_type)) ==
                    chess_game.interpret_stats.at(curr_player).end()) {
                    chess_game.interpret_stats.at(curr_player)[comments::rank(comment_type)] = 0.0;
                }

                chess_game.interpret_stats.at(curr_player)[comments::rank(comment_type)]++;

                const bool white_to_move = moving_player == Piece::White;

                // --move-symbols: the NAGs belong right after the move, before any comment
                if (opts.move_symbols != OutputMode::Apgn) {
                    const auto move_glyph     = comments::move_nag(comment_type);
                    const auto position_glyph = comments::position_nag(position_status, white_to_move);

                    if (!move_glyph.empty()) {
                        move_comment += " " + move_glyph;
                    }

                    if (!position_glyph.empty()) {
                        move_comment += " " + position_glyph;
                    }
                }

                // what goes inside the comment's braces. it can be empty: a move that delivers mate has no
                // eval tag, so with both std modes nothing is left, and then no comment is written at all
                std::string move_text;

                // --eval-format: the tag goes at the start of the comment, where readers expect it
                if (opts.eval_format != OutputMode::Apgn) {
                    move_text += eval_tag(mv_player_cp, mv_player_cm, white_to_move);
                }

                // in std mode the NAGs above replace apgn's own words
                if (opts.move_symbols != OutputMode::Std) {
                    move_text += comment;
                }

                // in std mode the tag above replaces apgn's own score text
                if (opts.eval_format != OutputMode::Std) {
                    move_text += score_text(mv_player_cp, mv_player_cm);
                }

                move_comment += move_text.empty() ? std::string(" ") : (" {" + move_text + " } ");

                chess_game.player_move_centipawn[i] = mv_player_cp;
                chess_game.best_move_centipawn[i]   = mv_best_cp;

                if (move != mv_best) {
                    std::string best_text;

                    if (opts.eval_format != OutputMode::Apgn) {
                        best_text += eval_tag(mv_best_cp, mv_best_cm, white_to_move);
                    }

                    // in std mode the NAGs replace apgn's words here too: the parentheses already mark the
                    // variation as the alternative, so only its score is written
                    const bool apgn_words = opts.move_symbols != OutputMode::Std;

                    if (apgn_words) {
                        best_text += " was the best move";
                    }

                    if (opts.eval_format != OutputMode::Std) {
                        best_text += score_text(mv_best_cp, mv_best_cm);
                    }

                    if (apgn_words) {
                        best_text += ", followed by";
                    }

                    move_comment += " (" + mv_best + (best_text.empty() ? std::string() : (" {" + best_text + " }")) +
                                    " " + best_mv_cont + " ) ";
                }

                // either search found a forced mate (for or against the player)
                const bool mate_found = (mv_player_cm != 0) || (mv_best_cm != 0);

                // --min-loss: a move that is less than `min_loss` centipawns worse than the engine's best
                // move is left unannotated. a move involving a forced mate is always annotated, because a
                // centipawn loss is not meaningful for it. the statistics count the move either way.
                const int centipawn_loss = mv_best_cp - mv_player_cp;

                if (opts.min_loss <= 0 || mate_found || centipawn_loss >= opts.min_loss) {
                    chess_game.comments[i] = move_comment;
                }

                // --stop-at-score: the game is decided, stop analysing the rest of it. the remaining moves stay
                // in the output without a comment, and the next game in the file is analysed as usual.
                if (opts.stop_at_score > 0) {
                    // the game's own move number, which a FEN position can start at any number
                    const int move_number   = chess_game.first_move_number + (i + chess_game.first_player) / 2;
                    const int white_view_cp = white_to_move ? mv_player_cp : -mv_player_cp;

                    // only the move played decides the game: a mate that the engine's best move would have
                    // forced, but the player missed, leaves the game open
                    if (mv_player_cm != 0) {
                        std::cout << "\nforced mate found on move " << move_number
                                  << ", skipping the rest of this game\n";
                        break;
                    }

                    if (std::abs(mv_player_cp) >= opts.stop_at_score) {
                        std::cout << "\nscore reached " << pawns_text(white_view_cp, true)
                                  << " pawns from white's point of view on move " << move_number
                                  << ", skipping the rest of this game\n";
                        break;
                    }
                }
            }

            DEBUG_LOG("**************** best move ****************");

            DEBUG_LOG("+> best move " + mv_best +
                      ((!mv_best_cp && mv_best_cm) ? (" checkmate in " + std::to_string(mv_best_cm))
                                                   : (" centipawn " + std::to_string(mv_best_cp))));

            DEBUG_LOG("*************** player move ***************");

            DEBUG_LOG("+> player move " + mv_player +
                      ((!mv_player_cp && mv_player_cm) ? (" checkmate in " + std::to_string(mv_player_cm))
                                                       : (" centipawn " + std::to_string(mv_player_cp))));

            DEBUG_LOG("*******************************************");

            prev_cm_in_nmoves.at(curr_player) = mv_player_cm;
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

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "\nanalysis done, killing " << chess_engine << " child process now...\n";
    std::cout << "analysis took " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count()
              << " sec(s)\n";

    // request uci chess engine exit

    std::string quit_uci = "quit\n";
    DEBUG_LOG(">>> send: " + quit_uci);
    asio::write(pipe_stdin, asio::buffer(quit_uci), ec);
    check_for_error("unable to properly close chess engine with 'quit': ", uci_process, ec);

    // let the engine exit by itself after "quit" (closing its stdin too), killing it right away made it
    // report exit status 15 (SIGTERM). it is only stopped forcefully if it is still running after a few seconds.
    pipe_stdin.close();

    bool engine_exited     = false;
    int process_return_int = 0;

    uci_process.async_wait([&](error_code wait_ec, int exit_code) {
        engine_exited      = true;
        process_return_int = wait_ec ? -1 : exit_code;
    });

    ctx.restart();
    ctx.run_for(std::chrono::seconds(5));

    if (!engine_exited) {
        std::cout << "warning: " << chess_engine << " did not exit after 'quit', stopping it\n";
        error_code terminate_ec;
        uci_process.terminate(terminate_ec);
        ctx.run(); // completes the pending wait
    }

    if (process_return_int) {
        std::cout << "uci chess engine return status : " << process_return_int << '\n';
        std::cout << "warning: an error might have occured\n";
    } else {
        std::cout << "uci engine return status : " << process_return_int << " <- no errors : success\n";
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
        auto moving_player = static_cast<Piece>((i + chess_game.first_player) % 2);

        analyzed_game.append(chess_game.moves[i]);

        // the last element of `moves` is the game result ("1-0", "*", ...), every move before it can have a comment
        // moves without a comment (e.g. skipped by --analyse-start-on-move) still need a separator
        if ((opts.piece == Piece::Both || opts.piece == moving_player) && i < chess_game.moves.size() - 1 &&
            chess_game.comments.find(i) != chess_game.comments.end()) {
            analyzed_game.append(chess_game.comments[i]);
        } else {
            analyzed_game.push_back(' ');
        }
    }

    analyzed_game.push_back('\n');
    std::cout << "analysis of current game done!\n\n";

    return analyzed_game;
}