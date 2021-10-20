#ifndef UCI_ANALYSER_INTERPRET
#define UCI_ANALYSER_INTERPRET

#include <iostream>
#include <iomanip>

namespace interpret
{
    void initializeOStream();
    std::string displayCP(int cpEval);
    void playedMove(int played_move, int best_move);
}

#endif