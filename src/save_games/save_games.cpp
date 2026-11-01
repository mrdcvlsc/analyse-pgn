#include "../save_games/save_games.hpp"
#include "../utils/get_exe_dir.hpp"
#include "../utils/logger.hpp"
#include "../utils/process_utils.hpp"

#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/readable_pipe.hpp>
#include <boost/asio/writable_pipe.hpp>
#include <boost/process.hpp>
#include <boost/process/v2/process.hpp>
#include <boost/process/v2/stdio.hpp>
#include <boost/system/detail/error_code.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

void save_games(const std::string &output_filename, const std::string &long_algebraic_notation_games) {
    std::cout << "saving analyzed pgn file : " << output_filename << '\n';

    namespace asio    = boost::asio;
    namespace process = boost::process;
    namespace fs      = std::filesystem;

    asio::io_context ctx;
    asio::writable_pipe pipe_stdin{ctx};

    auto exe_pgn_extract = (fs::path(get_exe_dir()) / "bin" / "pgn-extract" /
#if defined(_WIN32)
                            "pgn-extract.exe"
#else
                            "pgn-extract"
#endif
        )
                               .string();

    DEBUG_LOG("CONVERTING UCI NOTATION TO PGN: " + long_algebraic_notation_games);

    std::cout << "converting long algebraic notation games to pgn...\n";

    auto child_process = process::process(ctx.get_executor(), exe_pgn_extract,
        {"-WsanPNBRQK", "--output", output_filename}, process::process_stdio{pipe_stdin, {}, {}});

    boost::system::error_code ec;

    asio::write(pipe_stdin, asio::buffer(long_algebraic_notation_games), ec);
    check_for_error("unable to pipe the long algebraic notation games to pgn-extract ", child_process, ec);

    // closing stdin tells pgn-extract the input is complete, it then finishes writing the file and exits by itself
    pipe_stdin.close();

    int exit_code = child_process.wait();
    if (exit_code) {
        throw std::runtime_error("pgn-extract exited with code " + std::to_string(exit_code) + " while writing '" +
                                 output_filename + "', the analysed games were not saved");
    }

    std::cout << "conversion done!\n";

    std::cout << "pgn game saved to " << output_filename << "!\n";
}
