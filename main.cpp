#include <iostream>
#include <vector>

#define VERSION "version 0.8"

#define FLAG_VERSION "--version"
#define FLAG_HELP "--help"
#define FLAG_ANALYSE "-analyse"

#define ANALYSE_ENGINE "-engine"
#define ANALYSE_THREADS "-threads"
#define ANALYSE_DEPTH "-depth"
#define ANALYSE_COLOR "-color"
#define ANALYSE_OPENNING_SKIP "-oskip"


std::string DEFAULT_ENGINE() {}
#define DEFAULT_THREAD 1
#define DEFAULT_DEPTH 12
#define DEFAULT_COLOR 'A'
#define DEAFULT_OPENNING_MOVE_SKIP 4

bool isNumber(const std::string& input) { /*number should not be negative, and should be a valid number*/ }
bool isEngine(const std::string& input) {}
bool isPGN(const std::string& input) { /*check is file is a vald pgn*/ }
bool isValidColor(const std::string& input) { /*color should just ba a one character string, options are [b,B,w,W,a,A]*/ }

int main(int argc, char* argv[])
{
    std::string engine = DEFAULT_ENGINE();
    int thread = DEFAULT_THREAD;
    int depth  = DEFAULT_DEPTH;
    char color = DEFAULT_COLOR;
    int openning_move_skip = DEAFULT_OPENNING_MOVE_SKIP;


    std::vector<std::string> ARGUMENTS;
    std::vector<std::string> PGN_GAMES;

    for(int i=1; i<argc; ++i) ARGUMENTS.push_back(argv[i]);

    // scan engine
    for(size_t i=1; i<argc; ++i)
    {
        if(isEngine(ARGUMENTS[i]))
        {
            engine = ARGUMENTS[i];
        }
    }

    // scan thread
    for(size_t i=1; i<argc; ++i)
    {
        if(ARGUMENTS[i]==ANALYSE_THREADS)
        {
            if(isNumber(ARGUMENTS[i+1]))
            {
                thread = std::atoi(ARGUMENTS[i+1].data());
            }
        }
    }

    // scan depth
    for(size_t i=1; i<argc; ++i)
    {
        if(ARGUMENTS[i]==ANALYSE_DEPTH)
        {
            if(isNumber(ARGUMENTS[i+1]))
            {
                depth = std::atoi(ARGUMENTS[i+1].data());
            }
        }
    }

    // scan color
    for(size_t i=1; i<argc; ++i)
    {
        if(ARGUMENTS[i]==ANALYSE_DEPTH)
        {
            if(isValidColor(ARGUMENTS[i+1]))
            {
                color = ARGUMENTS[i+1][0];
            }
        }
    }

    // scan opening skip count
    for(size_t i=1; i<argc; ++i)
    {
        if(ARGUMENTS[i]==ANALYSE_OPENNING_SKIP)
        {
            if(isNumber(ARGUMENTS[i+1]))
            {
                openning_move_skip = std::atoi(ARGUMENTS[i+1].data());
            }
        }
    }

    // scan pgn files
    for(size_t i=1; i<argc; ++i)
    {
        if(isPGN(ARGUMENTS[i]))
        {
            PGN_GAMES.push_back(ARGUMENTS[i]);
        }
    }

    // start analysing games
}