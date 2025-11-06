#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#endif

#include "../chess_games.hpp"
#include "../utils/process_utils.hpp"

#include <boost/asio.hpp>
#include <boost/asio/readable_pipe.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/process/process.hpp>
#include <boost/system/detail/error_code.hpp>

#include <iostream>
#include <string>
#include <utility>

using error_code  = boost::system::error_code;
namespace process = boost::process;
namespace asio    = boost::asio;

const int NOT_EXPECTED_LINE_OUTPUT_LIMIT = 100'000;

enum struct Piece : char {
    White,
    Black,
    Both,
};

struct UciOptions {
    int depth;
    int threads;
    int hash_size;
    int start_on_move; // start analysis on move N
    int until_move;    // analyse only until move N
    Piece piece;
};

std::string analyse_game(ChessGame &chess_game, const std::string &chess_engine, const UciOptions &opts);

// return the last 2 lines of a uci `go` command.
std::pair<std::string, std::string> get_ucigo_bestmove(process::process &uci_process,
    const std::string &go_command,
    asio::writable_pipe &pipe_stdin,
    asio::readable_pipe &pipe_stdout,
    asio::streambuf &child_stdout_buf,
    const UciOptions &opts);

/**
    @arg cp_pmvbmv_dif - player move centipawn - best move centipawn.
    @arg cp_pm - player move centipawn value.
    @arg cp_bm - best move centipawn value.
    @arg cm_pm - player move checkmate in N moves.
    @arg cm_bm - best move checkmate in N moves
*/
std::string score_comments(int cp_pmvbmv_dif, int cp_pmv, int cp_bmv, int cm_pmv, int cm_bmv);