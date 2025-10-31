#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#endif

#include "analyse_game.hpp"
#include "get_exe_dir.hpp"
#include "load_games.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// always put this at the end
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

int main() {
    namespace fs = std::filesystem;

    auto games = load_games((fs::current_path() / "pgn_samples" / "first.pgn").string());
    std::cout << "number of games : " << games.size() << '\n';

    for (const auto &game : games) {
        std::cout << "===========LD===================\n";

        for (const auto &tag : game.tags) {
            auto key = tag.first;
            auto val = tag.second;
            std::cout << key << ":" << val << '\n';
        }

        std::cout << '\n';

        for (const auto &move : game.moves) {
            std::cout << move << ' ';
        }

        std::cout << '\n';
    }

    std::cout << '\n';
    std::cout << '\n';

    auto chess_engine = (fs::path(get_exe_dir()) / "bin" / "engines" /
#if defined(_WIN32)
                         "stockfish.exe"
#else
                         "stockfish"
#endif
        )
                            .string();

    for (const auto &game : games) {
        std::cout << "==============================\n";
        std::cout << analyse_game(game, chess_engine) << '\n';
    }

    return 0;
}