#pragma once

#include "../chess_games.hpp"

#include <string>
#include <vector>

std::vector<ChessGame> load_games(const std::string &filename);