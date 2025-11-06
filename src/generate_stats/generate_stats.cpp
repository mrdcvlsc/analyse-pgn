#include "generate_stats.hpp"

#include "../analyse_game/score_comments.hpp"
#include "../utils/logger.hpp"

#include <glaze/glaze.hpp>

#include <array>
#include <glaze/json/prettify.hpp>
#include <limits>
#include <string>
#include <utility>
#include <vector>

void generate_stats(std::vector<ChessGame> &chess_games, const std::string &stat_file) {
    JsonStats json_stats;

    for (auto &chess_game : chess_games) {

        std::array<double, 2> total_player_score{0.0, 0.0};
        std::array<double, 2> total_player_moves{0.0, 0.0};
        std::array<double, 2> accuracies{0.0, 0.0};

        if (chess_game.player_move_centipawn.size() != chess_game.best_move_centipawn.size()) {
            std::cerr << "unable to generate stat file, best move and player move count didn't match\n";
            return;
        }

        GameStats game_stats;

        for (const auto &[tag, value] : chess_game.tags) {
            game_stats.tags.push_back(std::make_pair(tag, value));
        }

        std::vector<PlayerStats> player_stats;

        for (int i = 0; i < 2; i++) {
            PlayerStats player_stat;

            if (i == 0) {
                player_stat.player_color = "white";
            } else {
                player_stat.player_color = "black";
            }

            for (const auto [_, centipawn] : chess_game.player_move_centipawn) {
                player_stat.centipawns.push_back(static_cast<int>(centipawn));
            }

            for (const auto &[ranked_comment, cnt] : chess_game.interpret_stats.at(i)) {
                total_player_moves.at(i) += cnt;

                auto [rank, comment] = ranked_comment;

                if (comment.find(comments::winning::BRILLIANT.second) != std::string::npos) {
                    total_player_score.at(i) += (cnt * 1.00);
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
                    total_player_score.at(i) += (cnt * 0.10);
                } else if (comment.find(comments::losing::MISTAKE_3.second) != std::string::npos) {
                    total_player_score.at(i) += (cnt * 0.05);
                } else if (comment.find(comments::winning::MISSED.second) != std::string::npos) {
                    total_player_score.at(i) += (cnt * 0.75);
                } else if (comment.find(comments::losing::BLUNDER.second) != std::string::npos) {
                    total_player_score.at(i) += (cnt * 0.01);
                }

                player_stat.move_stats.push_back(std::make_pair(comment, cnt));
            }

            player_stat.accuracy = total_player_score.at(i) / total_player_moves.at(i);
            player_stats.push_back(player_stat);
        }

        game_stats.player_stats = player_stats;
        json_stats.games.push_back(game_stats);

        // calculate average

        for (int i = 0; i < 2; i++) {
            DEBUG_LOG("====================================================");

            accuracies.at(i) = total_player_score.at(i) / total_player_moves.at(i);
            DEBUG_LOG(std::string("Color ") + (i == 0 ? "White" : "Black") +
                      " accuracy : " + std::to_string(accuracies.at(i)) + "\n");

            DEBUG_LOG(std::string("Color ") + (i == 0 ? "White" : "Black") +
                      " player total score : " + std::to_string(total_player_score.at(i)) + "\n");
        }
    }

    std::string final_json = "";
    glz::prettify_json(glz::write_json(json_stats).value_or("error"), final_json);

    std::ofstream output_file(stat_file);

    if (output_file.is_open()) {
        output_file << final_json;
        output_file.close();
        std::cout << "stats written to '" << stat_file << "' successfully." << '\n';
    } else {
        std::cerr << "Error: Unable to open or create file '" << stat_file << "'\n";
    }
}
