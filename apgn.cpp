#include <iostream>
#include "convert.hpp"

int main(int argc, char* args[])
{
    std::string pgn_file, pgn_output;

    int threads = 1, depth = 12, skips = 0;

    std::cout << "APGN : CHESS ANALYSER \n";

    char c = 'c';

    std::cout << "INPUT the character 'a' to analyse and 'h' for help\n";
    std::cout<<"input : "; std::cin>>c;

    if(c=='a')
    {
        std::cout<<"TO START INPUT THE FOLLOWING PARAMETERS\n";

        std::cout<<"pgn's path with filename  : ";
        std::cin>>pgn_file;

        std::cout<<"output path with filename : ";
        std::cin>>pgn_file;

        std::cout<<"number of threads         :";
        std::cin>>pgn_file;

        std::cout<<"engine depth level        :";
        std::cin>>pgn_file;

        std::cout<<"openning moves to be skipped :";
        std::cin>>pgn_file;
    }
    else if(c=='h')
    {
        std::string help_info = "INFO:\nThis program analyses chess games, that is recorded in .PGN files\n"
            "this program can analyse, pgn file even it has multiple games in it, as long as they are valid pgn files\n"
            "pgn files have a certain structure, if your pgn file is not structured properly, you might need to\n"
            "fix it first before this program can analyse that pgn files without errors.\n\n\n"
            "WHAT ARE THE PARAMETERS NEEDED TO START ANALYZING?\n\n"
            "\tinorder to analyse pgn file, you first need to input certain parameters and these are\n\n"
            
            "\t - pgn's path with filename : \n"
            "\t\tex: C:/User/Documents/ChessGames/thePGN.pgn\n"
            "\t\t'C:/User/Documents/ChessGames/' is the file location\n"
            "\t\tand 'thePGN.pgn' is the chess game in pgn format.\n\n"

            "\t - output path with filename : \n"
            "\t\tthis will be the generated pgn file with comment analysis\n"
            "\t\tex: C:/User/Documents/ChessGames/analyzed.pgn\n"
            "\t\t'C:/User/Documents/ChessGames/analyzed' is the path where to save it\n"
            "\t\t'analyzed.pgn' is the filename to be saved\n\n"

            "\t - number of threads : \n"
            "\t\tthreads to be used by the chess engine...\n"
            "\t\ttypicaly a basic modern computer usually have 4 threads\n"
            "\t\tminimum value is 1, maximum value depends on the machine\n"
            "\t\tmore threads means faster analysis, though you should know\n"
            "\t\tyour computer's maximum threads first\n\n"

            "\t - engine depth level : \n"
            "\t\tminimum value is 1, maximum value depends on you\n"
            "\t\tthe bigger the depth, the stronger the analysis\n"
            "\t\talso bigger depth means longer analysis\n\n";

            "\t - opening moves to be skipped : \n"
            "\t\tthis are the moves you don't want to analyse at the beginning\n"
            "\t\tyou can set this parameter to zero,\n"
            "\t\tif you want to analyse even the openning moves\n\n\n"

            "INSTALLING NEW CHESS ENGINE\n\n"
            "\n\nby default apgn uses the 'stockfish10' chess engine\n"
            "but you can change the engine you want to use... here are the steps\n\n"

            "step 1 : \n"
            "\tfind and download a chess engine executable over the internet\n"
            "\tmake sure you download the right version for your architecture\n";
            "\tthere are x64, x86, and x86_64 architectures available\n";
            "\tchoose the one that is compatible with your device\n\n";

            "step 2 : \n"
            "\tafter getting a compatible chess engine for your device\n";
            "\tput the executable into the ./apgn/bin/engine folder\n"
            "\trename it into something easy to type and no spaces"

            "step 3 : \n"
            "\tafter putting it the the engine folder, you should specify the engine\n"
            "\tthat is being used in the env.txt, look for the 'CHESS_ENGINE=stockfish10'\n"
            "\tand change the 'stockfish10' into the name of the engine that you put in\n"
            "\tthe engine folder."

            "step 4 : your done and can use that engine now for analysis\n";

        std::cout<<help_info<<"\n";
    }


    #if defined(__linux__)

    #elif defined(_WIN32)

    #endif

    return 0;
}