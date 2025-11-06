#pragma once

#include "../chess_games.hpp"

#include <array>
#include <optional>
#include <string>
#include <utility>
#include <vector>

struct PlayerStats {
    std::string player_color;
    double accuracy;
    std::vector<std::pair<std::string, int>> move_stats;
    std::vector<int> centipawns;
};

struct GameStats {
    std::vector<std::pair<std::string, std::string>> tags;
    std::vector<PlayerStats> player_stats;
};

struct JsonStats {
    std::vector<GameStats> games;
};

void generate_stats(std::vector<ChessGame> &chess_game, const std::string &stat_file);