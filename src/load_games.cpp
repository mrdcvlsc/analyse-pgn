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

#include "chess_games.hpp"
#include "get_exe_dir.hpp"

std::vector<ChessGame> load_games(const std::string &filename) {
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

    auto child_process = process::process(ctx.get_executor(), pgn_extract,
        // {"-Wlalg", "--nocomments", "--nonags", "--nomovenumbers", "--nochecks", filename},
        {"-Wlalg", "--nocomments", "--nomovenumbers", filename},
        process::process_stdio{{}, pipe_stdout, {}});

    std::string std_output;
    boost::system::error_code ec;

    asio::read(pipe_stdout, asio::dynamic_buffer(std_output), ec);
    if (!ec || (ec != asio::error::eof)) {
        std::cerr << "asio::dynamic_buffer result : " << ec.value() << " : " << ec.message()
                  << '\n';
    }

    int exit_code = child_process.wait();

    std::cout << "PGN FILE:\n";
    std::cout << std_output << '\n';
    std::cout << "-----eof-----\n";

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
                curr_game.tags[tag] = val;
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

                if (move == "1-0" || move == "0-1" || move == "1/2-1/2" || move == "*") {
                    in_headers = in_moves = false;
                    games.push_back(curr_game);
                    curr_game = ChessGame();
                    break;
                }

                curr_game.moves.push_back(move);
            }
        }
    }

    child_process.wait();
    return games;
}
