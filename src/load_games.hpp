#pragma once

#include <string>
#include <vector>

#include "chess_games.hpp"

std::vector<ChessGame> load_games(const std::string &filename);