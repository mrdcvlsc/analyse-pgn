#ifndef UCI_ANALYSER_INTERPRET
#define UCI_ANALYSER_INTERPRET

#include <iostream>
#include <iomanip>
#include <fstream>

namespace interpret
{
    void initializeOStream();
    void clearStats();
    void saveStats(const std::string& saveFile, bool color);
    std::string displayCP(int cpEval);
    void playedMove(int played_move, int best_move, bool color);
}

#endif