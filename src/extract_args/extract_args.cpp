#include "extract_args.hpp"
#include "checks.hpp"
#include "get_defaults.hpp"

#include "../utils/get_exe_dir.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

// a pgn file is recognised by its extension (".pgn", any letter case), not by "pgn" appearing anywhere in the path
static bool is_pgn_file(const std::string &argument) {
    auto extension = std::filesystem::path(argument).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    return extension == ".pgn";
}

// the longest possible legal chess game is under 9,000 moves
static const int MAX_MOVE_NUMBER = 10'000;

// converts a flag value that is_number() accepted (digits only) and exits with an error
// when it is outside [min, max], so nonsense values never reach the engine or overflow `int`
static int flag_number(const std::string &flag, const std::string &value, int min, int max) {
    const auto first_digit = value.find_first_not_of('0');
    const auto digits      = (first_digit == std::string::npos) ? std::string("0") : value.substr(first_digit);

    // more than 10 digits cannot be a valid value, compare the rest as a 64-bit number
    const long long number = (digits.size() > 10) ? static_cast<long long>(max) + 1 : std::stoll(digits);

    if (number < min || number > max) {
        std::cerr << "Argument Error: " << flag << " " << value << "\n\n\tthe value of " << flag
                  << " must be between " << min << " and " << max << "\n";
        exit(1);
    }

    return static_cast<int>(number);
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
                options.threads = flag_number(arguments[i - 1], arguments[i], 1, 1024);
            } else {
                assert_invalid("threads value", arguments[i - 1], arguments[i]);
            }
        } else if (arguments[i] == "--depth") {
            assert_missing_flagvalue(i, arguments.size(), arguments[i]);
            if (is_number(arguments[++i])) {
                options.depth = flag_number(arguments[i - 1], arguments[i], 1, 100);
            } else {
                assert_invalid("depth value", arguments[i - 1], arguments[i]);
            }
        } else if (arguments[i] == "--hash-size") {
            assert_missing_flagvalue(i, arguments.size(), arguments[i]);
            if (is_number(arguments[++i])) {
                options.hash_size = flag_number(arguments[i - 1], arguments[i], 1, 33'554'432);
            } else {
                assert_invalid("hash-size value", arguments[i - 1], arguments[i]);
            }
        } else if (arguments[i] == "--player") {
            assert_missing_flagvalue(i, arguments.size(), arguments[i]);
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
                options.start_on_move = flag_number(arguments[i - 1], arguments[i], 0, MAX_MOVE_NUMBER);
            } else {
                assert_invalid("analyse-start-on-move value", arguments[i - 1], arguments[i]);
            }
        } else if (arguments[i] == "--analyse-until-move") {
            assert_missing_flagvalue(i, arguments.size(), arguments[i]);
            if (is_number(arguments[++i])) {
                options.until_move = flag_number(arguments[i - 1], arguments[i], 1, MAX_MOVE_NUMBER);
            } else {
                assert_invalid("analyse-until-move value", arguments[i - 1], arguments[i]);
            }
        } else if (is_pgn_file(arguments[i])) {
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
              << "\nthreads                : " << options.threads << "\ndepth                  : " << options.depth
              << "\nhash size              : " << options.hash_size << "\nplayer to analyse      : " << piece
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
            std::cout << number++ << ".) " << pgn_files.string() << '\n';
        }
    }

    std::cout << "\n==============================================\n";

    return std::make_pair(arg_paths, options);
}