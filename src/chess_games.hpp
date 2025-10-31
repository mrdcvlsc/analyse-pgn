#pragma once

#include <map>
#include <string>
#include <vector>

struct ChessGame {
    std::map<std::string, std::string> tags;
    std::vector<std::string> moves;
    std::map<int, std::string> comments; // map = key : move index, value: comment
};