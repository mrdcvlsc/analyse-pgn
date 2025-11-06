#include "extract_args.hpp"
#include "../utils/get_exe_dir.hpp"

#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

bool is_number(const std::string &input) {
    std::string::const_iterator i = input.begin();
    while (i != input.end() && std::isdigit(*i)) {
        ++i;
    }

    return !input.empty() && i == input.end();
}

std::pair<std::string, UciOptions> get_defaults() {

    auto chess_engine = (fs::path(get_exe_dir()) / "bin" / "engines" /
#if defined(_WIN32)
                         "stockfish.exe"
#else
                         "stockfish"
#endif
        )
                            .string();

    auto thread_cnt     = std::thread::hardware_concurrency();
    auto threads_to_use = static_cast<int>(static_cast<float>(thread_cnt) * 0.75f);

    if (threads_to_use <= 0 || threads_to_use > 512) {
        threads_to_use = 1;
    }

    DEBUG_LOG("Threads to use by the chess engine : " + std::to_string(threads_to_use));

    auto options = UciOptions{};

    options.depth         = 11;
    options.threads       = threads_to_use;
    options.hash_size     = 850;
    options.start_on_move = 0;
    options.until_move    = 1'000;
    options.piece         = Piece::Both;

    return std::make_pair(chess_engine, options);
}

std::pair<ArgPaths, UciOptions> extract_args(int argc, char *argv[]) {

    auto default_values   = get_defaults();
    auto chess_engine_exe = default_values.first;
    auto options          = default_values.second;

    auto arg_paths = ArgPaths{};

    std::vector<std::string> arguments;

    for (int i = 1; i < argc; i++) {
        arguments.push_back(argv[i]);
    }

    for (const auto &e : arguments) {
        std::cout << "argument pushed back : " << e << '\n';
    }

    auto piece = "Both";

    for (int i = 0; i < arguments.size(); i++) {
        if (arguments[i] == "--engine") {
            assert_missing_flagvalue(i, arguments.size(), arguments[i]);
            if (std::filesystem::exists(arguments[++i])) {
                chess_engine_exe = arguments[i];
            } else {
                assert_invalid("engine value", arguments[i - 1], arguments[i]);
            }
        } else if (arguments[i] == "--threads") {
            assert_missing_flagvalue(i, arguments.size(), arguments[i]);
            if (is_number(arguments[++i])) {
                options.threads = std::atoi(arguments[i].data());
            } else {
                assert_invalid("threads value", arguments[i - 1], arguments[i]);
            }
        } else if (arguments[i] == "--depth") {
            assert_missing_flagvalue(i, arguments.size(), arguments[i]);
            if (is_number(arguments[++i])) {
                options.depth = std::atoi(arguments[i].data());
            } else {
                assert_invalid("depth value", arguments[i - 1], arguments[i]);
            }
        } else if (arguments[i] == "--hash-size") {
            assert_missing_flagvalue(i, arguments.size(), arguments[i]);
            if (is_number(arguments[++i])) {
                options.hash_size = std::atoi(arguments[i].data());
            } else {
                assert_invalid("hash-size value", arguments[i - 1], arguments[i]);
            }
        } else if (arguments[i] == "--player") {
            i++;

            if (arguments[i] == "Both") {
                options.piece = Piece::Both;
                piece         = "Both";
            } else if (arguments[i] == "White") {
                options.piece = Piece::White;
                piece         = "White";
            } else if (arguments[i] == "Black") {
                options.piece = Piece::Black;
                piece         = "Black";
            } else {
                assert_invalid("player value", arguments[i - 1], arguments[i]);
            }
        } else if (arguments[i] == "--analyse-start-on-move") {
            assert_missing_flagvalue(i, arguments.size(), arguments[i]);
            if (is_number(arguments[++i])) {
                options.start_on_move = std::atoi(arguments[i].data());
            } else {
                assert_invalid("analyse-start-on-move value", arguments[i - 1], arguments[i]);
            }
        } else if (arguments[i] == "--analyse-until-move") {
            assert_missing_flagvalue(i, arguments.size(), arguments[i]);
            if (is_number(arguments[++i])) {
                options.until_move = std::atoi(arguments[i].data());
            } else {
                assert_invalid("analyse-until-move value", arguments[i - 1], arguments[i]);
            }
        } else if (std::string(arguments[i]).find("pgn") != std::string::npos) {
            auto pgn_file_arg = std::filesystem::path(arguments[i]);
            std::cout << "pgn file detected : " << pgn_file_arg << '\n';
            arg_paths.chess_pgn_files.push_back(pgn_file_arg);
        } else {
            std::cout << "ignoring the unknown parameter '" << arguments[i] << "\n";
        }
    }

    arg_paths.chess_engine_exe = chess_engine_exe;

    std::cout << "\n================= flag values ================\n";

    std::cout << "\nengine                 : " << chess_engine_exe
              << "\nthreads                : " << options.threads
              << "\ndepth                  : " << options.depth
              << "\nhash size              : " << options.hash_size
              << "\nplayer to analyse      : " << piece
              << "\nstart analysis on move : " << options.start_on_move
              << "\nanalyse until move     : " << options.until_move << "\n\n";

    if (options.depth > 12) {
        std::cout << "NOTE: depth is set greater than 12!!!\n"
                     "      analysis might take longer...\n\n";
    }

    if (arg_paths.chess_pgn_files.size() == 0) {
        throw std::invalid_argument("no pgn files detected to analyse, early exit");
    } else {
        std::cout << "\n============ pgn files to analyse ============\n";

        int number = 1;
        for (const auto &pgn_files : arg_paths.chess_pgn_files) {
            std::cout << number << ".) " << pgn_files.string() << '\n';
        }
    }

    std::cout << "\n==============================================\n";

    return std::make_pair(arg_paths, options);
}

void assert_missing_flagvalue(int index, std::size_t arg_cnt, const std::string &argument) {
    if (index + 1 >= arg_cnt) {
        std::cerr << "INVALID ARGUMENT:\n\n\tThe given flag" << argument << "' does not have a value\n\n";
        exit(1);
    }
}

void assert_invalid(const std::string &what, const std::string &param_flag, const std::string &value) {
    std::cerr << "Argument Error: " << param_flag << " " << value
              << "\n\n"
                 "\tDetected invalid "
              << what
              << " given\n"
                 "\tin the parameter flag "
              << param_flag << "\n";
    exit(1);
}