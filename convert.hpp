#ifndef APGN_CONVERT_HPP
#define APGN_CONVERT_HPP

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <stdlib.h>
#include <sys/types.h>

#include <string.h>
#include <fstream>

#include "apgnFileSys.hpp"

#if defined(__linux__)
#include <unistd.h>
#include <wait.h>
#else
#include "winProcRun.hpp"
#endif

namespace apgn_convert
{
    namespace COLOR
    {
        const char ALL = 'A';
        const char WHITE = 'W';
        const char BLACK = 'B';
    }
    
    int run_subprog(std::string program, char *const args[]);

    void uci_to_pgn(const std::string& input, const std::string output);

    void pgn_to_uci(const std::string& input, const std::string output);

    void analyse_game(
        std::string input,
        std::string output,
        std::string engine,
        char* search_depth,
        char* threads,
        char* opening_move_skips,
        char* moves_until,
        char apgn_COLOR
    );
}

#endif