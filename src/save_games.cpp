#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/readable_pipe.hpp>
#include <boost/process.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/process/v2/stdio.hpp>
#include <boost/system/detail/error_code.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <utility>

#include "chess_games.hpp"
#include "get_exe_dir.hpp"
#include "logger.hpp"
#include "process.hpp"

void save_games(const std::string &filename, const std::vector<ChessGame> &games) {
    std::cout << "saving analyzed pgn file : " << filename << '\n';

    namespace asio = boost::asio;
    namespace process = boost::process;
    namespace fs = std::filesystem;

    asio::io_context ctx;
    asio::readable_pipe pipe_stdout{ctx};

    auto pgn_extract = (fs::path(get_exe_dir()) / "bin" / "pgn-extract" /
#if defined(_WIN32)
                        "pgn-extract.exe"
#else
                        "pgn-extract"
#endif
        )
                           .string();

    std::cout << "converting '" + filename + "' file to long-algebraic notation...\n";

    auto child_process = process::process(ctx.get_executor(), pgn_extract,
        {"-Wlalg", "--nocomments", "--nonags", "--nomovenumbers", "--nochecks", filename},
        // {"-Wlalg", "--nocomments", "--nomovenumbers", filename},
        process::process_stdio{{}, pipe_stdout, {}});

    std::string std_output;
    boost::system::error_code ec;

    asio::read(pipe_stdout, asio::dynamic_buffer(std_output), ec);
    int exit_code = child_process.wait();

    std::cout << "conversion of '" + filename + "' file to long-algebraic notation done!\n";

    if (ec && ec != asio::error::broken_pipe) {
        // broken pipe is normal here since the program ends after
        // reading every standard output from the child process.
        check_for_error("unable to run pgn-extract: " + ec.message(), child_process, ec);
    }

    DEBUG_LOG("PGN FILE:\n");
    DEBUG_LOG(std_output + '\n');
    DEBUG_LOG("-----eof-----\n");

    std::vector<ChessGame> games;
    ChessGame curr_game;

    std::istringstream process_output(std_output);
    std::string line;

    bool in_headers = false, in_moves = false;
    auto npos = std::string::npos;

    while (std::getline(process_output, line)) {
        if (line.empty()) {
            continue;
        }

        if (line.front() == '[' && !in_headers && !in_moves) {
            curr_game = ChessGame();
            in_headers = true;
        }

        if (in_headers && line.front() == '[') {
            auto tag_end = line.find(' ');
            auto val_start = line.find('"');
            auto val_end = line.rfind('"');

            if (tag_end != npos && val_start != npos && val_end != npos && val_end > val_start) {
                std::string tag = line.substr(1, tag_end - 1);
                std::string val = line.substr(val_start + 1, val_end - val_start - 1);
                curr_game.tags.push_back(std::make_pair(tag, val));
            }

            continue;
        }

        if (in_headers && !in_moves && line.front() != '[') {
            in_headers = false;
            in_moves = true;
        }

        if (in_moves) {
            std::istringstream line_of_moves(line);
            std::string move;

            while (line_of_moves >> move) {
                if (move.find('.') != npos) {
                    continue;
                }

                curr_game.moves.push_back(move);

                if (move == "1-0" || move == "0-1" || move == "1/2-1/2" || move == "*") {
                    in_headers = in_moves = false;
                    games.push_back(curr_game);
                    curr_game = ChessGame();
                    break;
                }
            }
        }
    }

    std::cout << filename << " game loaded to program successfully!\n";

    return games;
}
