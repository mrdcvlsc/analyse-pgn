#pragma once

#include <cstddef>
#include <string>

// writes `game_count` analysed games to `filename` in standard notation, and returns how many of them pgn-extract
// rejected (and so left out of the file)
std::size_t save_games(
    const std::string &filename, const std::string &long_algebraic_notation_games, std::size_t game_count);
