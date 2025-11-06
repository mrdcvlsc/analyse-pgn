#include "generate_stats.hpp"

#include "../analyse_game/score_comments.hpp"
#include "../utils/logger.hpp"

#include <array>
#include <limits>
#include <string>

struct PlayerStats {};

struct GameStats {};

void generate_stats(std::vector<ChessGame> &chess_games, const std::string &stat_file) {
    for (auto &chess_game : chess_games) {
        std::string stat_report = "";

        std::array<double, 2> total_player_score{0.0, 0.0};
        std::array<double, 2> total_player_moves{0.0, 0.0};
        std::array<double, 2> accuracies{0.0, 0.0};

        if (chess_game.player_move_centipawn.size() != chess_game.best_move_centipawn.size()) {
            std::cerr << "unable to generate stat file, best move and player move count didn't match\n";
            return;
        }

        for (int i = 0; i < 2; i++) {
            for (const auto &[ranked_comment, cnt] : chess_game.interpret_stats.at(i)) {
                total_player_moves.at(i) += cnt;

                auto [rank, comment] = ranked_comment;

                if (comment.find(comments::winning::BRILLIANT.second) != std::string::npos) {
                    total_player_score.at(i) += (cnt * 1.0);
                } else if (comment.find(comments::winning::EXCELLENT.second) != std::string::npos) {
                    total_player_score.at(i) += (cnt * 0.98);
                } else if (comment.find(comments::winning::GOOD.second) != std::string::npos) {
                    total_player_score.at(i) += (cnt * 0.97);
                } else if (comment.find(comments::losing::ACCURATE_3.second) != std::string::npos) {
                    total_player_score.at(i) += (cnt * 0.80);
                } else if (comment.find(comments::losing::ACCURATE_2.second) != std::string::npos) {
                    total_player_score.at(i) += (cnt * 0.77);
                } else if (comment.find(comments::losing::QUESTIONABLE.second) != std::string::npos) {
                    total_player_score.at(i) += (cnt * 0.75);
                } else if (comment.find(comments::losing::INACCURATE.second) != std::string::npos) {
                    total_player_score.at(i) += (cnt * 0.15);
                } else if (comment.find(comments::losing::MISTAKE_2.second) != std::string::npos) {
                    total_player_score.at(i) += (cnt * 0.1);
                } else if (comment.find(comments::losing::MISTAKE_3.second) != std::string::npos) {
                    total_player_score.at(i) += (cnt * 0.05);
                } else if (comment.find(comments::winning::MISSED.second) != std::string::npos) {
                    total_player_score.at(i) += (cnt * 0.75);
                } else if (comment.find(comments::losing::BLUNDER.second) != std::string::npos) {
                    total_player_score.at(i) += (cnt * 0.0);
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

            for (const auto &[ranked_comment, val] : chess_game.interpret_stats.at(i)) {
                auto [rank, comment] = ranked_comment;
                std::cout << "[" << comment << "]" << val << '\n';
            }
        }
    }

    // auto  ec = glz::write_file_json(obj, "./obj.json", std::string{});

    //     if (ec) {
    //   // handle error
    // }
}
