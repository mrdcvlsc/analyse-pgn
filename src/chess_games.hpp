#pragma once

#include <array>
#include <map>
#include <string>
#include <utility>
#include <vector>

struct ChessGame {
    /* -- the two members below are generated in the `load_games` function -- */

    std::vector<std::pair<std::string, std::string>> tags;
    std::vector<std::string> moves;

    /* -- the four members below are populated by the `analyse_game` function -- */

    std::map<int, std::string> comments;         // map = key : move index, value: comment.
    std::map<int, double> player_move_centipawn; // both player's moves centipawn values.
    std::map<int, double> best_move_centipawn;   // both player's missed best moves centipawn values.
    std::array<std::map<std::string, double>, 2> interpret_stats;
};