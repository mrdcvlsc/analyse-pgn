#ifndef APGN_CONVERT_HPP
#define APGN_CONVERT_HPP

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <boost/process.hpp>
#include <boost/asio.hpp>

#include "apgnFileSys.hpp"

namespace apgn_convert
{
    namespace COLOR
    {
        const char ALL = 'A';
        const char WHITE = 'W';
        const char BLACK = 'B';
    }
    
    int run_process(const std::string& program, const std::vector<std::string>& args);

    void uci_to_pgn(const std::string& input, const std::string& output);

    void pgn_to_uci(const std::string& input, const std::string& output);

    void analyse_game(
        const std::string& input,
        const std::string& output,
        const std::string& engine,
        int search_depth,
        int threads,
        int opening_move_skips,
        int moves_until,
        char color
    );
}

#endif