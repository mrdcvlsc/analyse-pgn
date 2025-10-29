#include "chess_games.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// always put this at the end
#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

int main() {

    namespace fs = std::filesystem;

    auto games = load_games((fs::current_path() / "pgn_samples" / "first.pgn").string());

    std::cout << "number of games : " << games.size() << '\n';

    for (const auto &game : games) {
        std::cout << "==============================\n";

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

    return 0;
}