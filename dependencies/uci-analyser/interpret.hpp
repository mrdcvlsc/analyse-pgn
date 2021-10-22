#ifndef UCI_ANALYSER_INTERPRET
#define UCI_ANALYSER_INTERPRET

#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits.h>

#if defined(__linux__)
#include <libgen.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#include <locale>
#include <codecvt>
#endif

namespace interpret
{
    void initializeOStream();
    void clearStats();
    void recordStats(const std::string& saveFile, bool color, size_t gameNumber);
    std::string displayCP(int cpEval);
    void playedMove(int played_move, int best_move, bool color);
    std::string get_execpath();
}

#endif