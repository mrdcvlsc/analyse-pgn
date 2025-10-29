#pragma once

#include <map>
#include <string>
#include <vector>

struct ChessGame {
    std::map<std::string, std::string> tags;
    std::vector<std::string> moves;
};

std::vector<ChessGame> load_games(const std::string &filename);