#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#endif

#include "analyse_game/analyse_game.hpp"
#include "extract_args/extract_args.hpp"
#include "generate_stats/generate_stats.hpp"
#include "info/display_info.hpp"
#include "load_games/load_games.hpp"
#include "save_games/save_games.hpp"
#include "utils/get_exe_dir.hpp"
#include "utils/logger.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

// always put this at the end
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

    if (argc == 1) {
        print_info();
        return 0;
    } else if (argc == 2) {
        if (strcmp(argv[1], "--help") == 0) {
            print_help();
        } else if (strcmp(argv[1], "--version") == 0) {
            print_version();
        }
        return 0;
    }

    auto args             = extract_args(argc, argv);
    auto chess_engine_exe = args.first.chess_engine_exe;
    auto chess_pgn_files  = args.first.chess_pgn_files;
    auto options          = args.second;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "analyse pgn (apgn) program starting...\n";

    for (const auto &pgn_file : chess_pgn_files) {
        std::cout << "loading chess games in " << pgn_file.string() << " file...\n";
        auto chess_games = load_games(pgn_file.string());

        std::string analyzed_games = "";
        for (auto &chess_game : chess_games) {
            analyzed_games += analyse_game(chess_game, chess_engine_exe, options) + '\n';
        }

        auto analyzed_pgn_file = (pgn_file.parent_path() / ("analyzed-" + pgn_file.filename().string())).string();
        save_games(analyzed_pgn_file, analyzed_games);

        auto stats_file =
            (pgn_file.parent_path() / ("analyzed-" + pgn_file.filename().replace_extension(".stat").string()))
                .string();
        generate_stats(chess_games, stats_file);
    }

    return 0;
}