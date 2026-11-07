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

// --outputnag and --evaltags take one of "std", "apgn" or "both"
static OutputMode flag_output_mode(const std::string &flag, const std::string &value) {
    if (value == "apgn") {
        return OutputMode::Apgn;
    } else if (value == "std") {
        return OutputMode::Std;
    } else if (value == "both") {
        return OutputMode::Both;
    }

    assert_invalid(flag.substr(2) + " value", flag, value); // exits
    return OutputMode::Apgn;
}

static std::string output_mode_text(OutputMode mode) {
    switch (mode) {
    case OutputMode::Std:
        return "std";
    case OutputMode::Both:
        return "both";
    default:
        return "apgn";
    }
}

// the longest possible legal chess game is under 9,000 moves
static const int MAX_MOVE_NUMBER = 10'000;

// "0.50" for 50 centipawns, for messages about flags whose value is given in pawns
static std::string pawns_text(int centipawns) {
    const auto whole     = centipawns / 100;
    const auto remainder = centipawns % 100;

    return std::to_string(whole) + "." + ((remainder < 10) ? "0" : "") + std::to_string(remainder);
}

// converts a flag value in pawns that is_pawn_number() accepted ("0.5", "2.00") into centipawns,
// and exits with an error when it is outside [min_centipawns, max_centipawns]
static int flag_pawns(const std::string &flag, const std::string &value, int min_centipawns, int max_centipawns) {
    const auto dot        = value.find('.');
    const auto whole_text = value.substr(0, dot);

    auto fraction_text = (dot == std::string::npos) ? std::string() : value.substr(dot + 1);
    fraction_text.resize(2, '0'); // "5" is 50 centipawns, "05" is 5

    // a whole part this long cannot be in range, and would overflow the conversion below
    const long long whole = (whole_text.size() > 9) ? static_cast<long long>(max_centipawns) + 1
                                                    : std::stoll(whole_text);

    const long long centipawns = whole * 100 + std::stoll(fraction_text);

    if (centipawns < min_centipawns || centipawns > max_centipawns) {
        std::cerr << "Argument Error: " << flag << " " << value << "\n\n\tthe value of " << flag
                  << " must be between " << pawns_text(min_centipawns) << " and " << pawns_text(max_centipawns)
                  << " pawns\n";
        exit(1);
    }

    return static_cast<int>(centipawns);
}

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
        } else if (arguments[i] == "--threshold") {
            assert_missing_flagvalue(i, arguments.size(), arguments[i]);
            if (is_pawn_number(arguments[++i])) {
                options.threshold = flag_pawns(arguments[i - 1], arguments[i], 0, 1'000);
            } else {
                assert_invalid("threshold value", arguments[i - 1], arguments[i]);
            }
        } else if (arguments[i] == "--limitscore") {
            assert_missing_flagvalue(i, arguments.size(), arguments[i]);
            if (is_pawn_number(arguments[++i])) {
                options.limit_score = flag_pawns(arguments[i - 1], arguments[i], 1, 10'000);
            } else {
                assert_invalid("limitscore value", arguments[i - 1], arguments[i]);
            }
        } else if (arguments[i] == "--outputnag") {
            assert_missing_flagvalue(i, arguments.size(), arguments[i]);
            i++;
            options.nag_mode = flag_output_mode(arguments[i - 1], arguments[i]);
        } else if (arguments[i] == "--evaltags") {
            assert_missing_flagvalue(i, arguments.size(), arguments[i]);
            i++;
            options.eval_mode = flag_output_mode(arguments[i - 1], arguments[i]);
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
              << "\nanalyse until move     : " << options.until_move
              << "\nannotation threshold   : " << pawns_text(options.threshold) << " pawns"
              << "\nscore limit            : "
              << ((options.limit_score > 0) ? (pawns_text(options.limit_score) + " pawns") : std::string("off"))
              << "\nmove assessment output : " << output_mode_text(options.nag_mode)
              << "\nscore output           : " << output_mode_text(options.eval_mode) << "\n\n";

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