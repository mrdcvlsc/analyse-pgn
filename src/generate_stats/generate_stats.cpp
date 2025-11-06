#include "generate_stats.hpp"
#include "../utils/logger.hpp"

#include <array>
#include <limits>
#include <string>

void generate_stats(std::vector<ChessGame> &chess_games, const std::string &stat_file) {
    for (auto &chess_game : chess_games) {
        std::string stat_report = "";

        std::array<double, 2> total_player_score{0.0, 0.0};
        std::array<double, 2> total_player_moves{0, 0};
        std::array<double, 2> accuracies{0.0, 0.0};

        if (chess_game.player_move_centipawn.size() != chess_game.best_move_centipawn.size()) {
            std::cerr << "unable to generate stat file, best move and player move count didn't match\n";
            return;
        }

        for (int i = 0; i < 2; i++) {
            for (const auto [j, centipawn] : chess_game.player_move_centipawn) {
                if (j % 2 != i) {
                    continue;
                }

                auto cp_best = chess_game.best_move_centipawn.at(j);
                auto cp_play = chess_game.player_move_centipawn.at(j);
                auto cp_diff = cp_play - cp_best;

                total_player_moves.at(i)++;

                auto base_score = 1.0;

                if (cp_play < 0) {
                    base_score = 0.9;
                }

                if (cp_diff > 0) {
                    total_player_score.at(i) += (base_score * 1.00);
                } else if (cp_diff > -30) {
                    total_player_score.at(i) += (base_score * 0.90);
                } else if (cp_diff > -40) {
                    total_player_score.at(i) += (base_score * 0.80);
                } else if (cp_diff > -50) {
                    total_player_score.at(i) += (base_score * 0.75);
                }
            }
        }

        // calculate average

        for (int i = 0; i < 2; i++) {
            DEBUG_LOG("====================================================");

            accuracies.at(i) = total_player_score.at(i) / total_player_moves.at(i);
            DEBUG_LOG(std::string("Color ") + (i == 0 ? "White" : "Black") +
                      " accuracy : " + std::to_string(accuracies.at(i)) + "\n");

            DEBUG_LOG(std::string("Color ") + (i == 0 ? "White" : "Black") +
                      " player total score : " + std::to_string(total_player_score.at(i)) + "\n");

            DEBUG_LOG(std::string("Color ") + (i == 0 ? "White" : "Black") +
                      " player moves : " + std::to_string(total_player_moves.at(i)) + "\n");

            DEBUG_LOG(std::string("Color ") + (i == 0 ? "White" : "Black") +
                      " game move size : " + std::to_string(chess_game.player_move_centipawn.size()) + "\n");

            DEBUG_LOG(std::string("Color ") + (i == 0 ? "White" : "Black") +
                      " game best move size : " + std::to_string(chess_game.best_move_centipawn.size()) + "\n");
        }
    }
}
