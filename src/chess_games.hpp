#pragma once

#include <map>
#include <string>
#include <vector>
#include <utility>

struct ChessGame {
    std::vector<std::pair<std::string, std::string>> tags;
    std::vector<std::string> moves;
    std::map<int, std::string> comments; // map = key : move index, value: comment
};