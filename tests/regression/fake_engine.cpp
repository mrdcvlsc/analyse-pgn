// A scripted UCI chess engine for the apgn regression tests.
//
// It is built by CMake next to apgn (target `fake_engine`) and passed to the
// tests as FAKE_ENGINE. apgn starts the engine without arguments, so the
// behaviour is driven by environment variables (inherited from apgn):
//
// FAKE_ENGINE_PLAN
//     Path to a plan file. Each non-empty line not starting with '#' is
//
//         <ply> <bestmove> <best_score> <player_score>
//
//     <ply> is the number of moves in the current position (0 = start).
//     Scores are written as cp:<n> or mate:<n>, from the side to move.
//     A plain `go` answers with <bestmove> and <best_score>; a
//     `go ... searchmoves <move>` answers with <move> and <player_score>.
//
// FAKE_ENGINE_MODE
//     normal                 (default) behave as described above.
//     exit_on_start          exit immediately, before printing anything.
//     info_string_after_pv   after the scored `info` line, print an
//                            `info string` line before `bestmove`, as
//                            real engines do (e.g. Stockfish NNUE notes).

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct PlannedSearch {
    std::string bestmove;
    std::string best_score;
    std::string player_score;
};

[[noreturn]] void fail(const std::string &message) {
    std::cerr << "fake engine: " << message << '\n';
    std::exit(2);
}

std::string environment(const char *name) {
    const char *value = std::getenv(name);
    return value ? value : "";
}

std::vector<std::string> split_words(const std::string &line) {
    std::istringstream stream(line);
    return {std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>()};
}

std::map<int, PlannedSearch> load_plan(const std::string &path) {
    std::map<int, PlannedSearch> plan;
    if (path.empty()) {
        return plan;
    }

    std::ifstream plan_file(path);
    if (!plan_file) {
        fail("cannot open plan file " + path);
    }

    std::string line;
    while (std::getline(plan_file, line)) {
        const auto words = split_words(line);
        if (words.empty() || words[0][0] == '#') {
            continue;
        }
        if (words.size() != 4) {
            fail("bad plan line: " + line);
        }
        plan[std::stoi(words[0])] = PlannedSearch{words[1], words[2], words[3]};
    }

    return plan;
}

// "cp:37" -> "cp 37", "mate:-2" -> "mate -2"
std::string uci_score(const std::string &token) {
    const auto colon = token.find(':');
    const auto kind  = token.substr(0, colon);
    if (colon == std::string::npos || (kind != "cp" && kind != "mate")) {
        fail("bad score token: " + token);
    }
    return kind + " " + token.substr(colon + 1);
}

void send(const std::string &line) {
    std::cout << line << '\n' << std::flush;
}

} // namespace

int main() {
    auto mode = environment("FAKE_ENGINE_MODE");
    if (mode.empty()) {
        mode = "normal";
    }
    if (mode != "normal" && mode != "exit_on_start" && mode != "info_string_after_pv") {
        fail("unknown FAKE_ENGINE_MODE: " + mode);
    }
    if (mode == "exit_on_start") {
        return 0;
    }

    const auto plan = load_plan(environment("FAKE_ENGINE_PLAN"));
    int ply         = 0;

    send("Fake UCI engine for apgn tests");

    std::string line;
    while (std::getline(std::cin, line)) {
        const auto words = split_words(line);
        if (words.empty()) {
            continue;
        }
        const auto &command = words[0];

        if (command == "uci") {
            send("id name FakeEngine");
            send("id author apgn tests");
            send("uciok");
        } else if (command == "isready") {
            send("readyok");
        } else if (command == "position") {
            const auto moves = std::find(words.begin(), words.end(), "moves");
            ply = (moves == words.end()) ? 0 : static_cast<int>(std::distance(moves, words.end()) - 1);
        } else if (command == "go") {
            const auto planned = plan.find(ply);
            if (planned == plan.end()) {
                std::cerr << "fake engine: no plan for ply " << ply << '\n';
                send("bestmove (none)");
                continue;
            }

            auto move  = planned->second.bestmove;
            auto score = planned->second.best_score;

            const auto searchmoves = std::find(words.begin(), words.end(), "searchmoves");
            if (searchmoves != words.end() && std::next(searchmoves) != words.end()) {
                move  = *std::next(searchmoves);
                score = planned->second.player_score;
            }

            send("info depth 1 seldepth 1 multipv 1 score " + uci_score(score) + " nodes 1 pv " + move);
            if (mode == "info_string_after_pv") {
                send("info string fake engine finished searching");
            }
            send("bestmove " + move);
        } else if (command == "quit") {
            return 0;
        }
    }

    return 0;
}
