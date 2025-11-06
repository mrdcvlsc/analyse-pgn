#include "get_defaults.hpp"
#include "../utils/get_exe_dir.hpp"

#include <filesystem>

namespace fs = std::filesystem;

std::pair<std::string, UciOptions> get_defaults() {

    auto chess_engine = (fs::path(get_exe_dir()) / "bin" / "engines" /
#if defined(_WIN32)
                         "stockfish.exe"
#else
                         "stockfish"
#endif
        )
                            .string();

    auto thread_cnt     = std::thread::hardware_concurrency();
    auto threads_to_use = static_cast<int>(static_cast<float>(thread_cnt) * 0.75f);

    if (threads_to_use <= 0 || threads_to_use > 512) {
        threads_to_use = 1;
    }

    DEBUG_LOG("Threads to use by the chess engine : " + std::to_string(threads_to_use));

    auto options = UciOptions{};

    options.depth         = 11;
    options.threads       = threads_to_use;
    options.hash_size     = 850;
    options.start_on_move = 0;
    options.until_move    = 1'000;
    options.piece         = Piece::Both;

    return std::make_pair(chess_engine, options);
}