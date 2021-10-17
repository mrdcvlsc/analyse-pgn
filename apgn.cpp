#include <iostream>
#include <vector>
#include <exception>

#include "convert.hpp"
#include "envgrabber.hpp"
#include "path.hpp"

#define CHESS_ENGINE 0
#define WORKER_THREADS 1
#define ANALYSIS_DEPTH 2
#define OPENING_MOVE_TO_SKIP 3

#define INSTALL_DIR "undefined"

void errorFileNotFound(const std::string& filename)
{
    std::FILE *filereader = std::fopen(filename.c_str(), "rb");
    if(!filereader)
    {
        throw std::invalid_argument(
            "The given file '"+filename+"' was not found"
        );
    }
}

void deleteFile(const std::string& filename)
{
    try {
        if (std::filesystem::remove(filename))
            std::cout << "file " << filename << " deleted.\n";
        else
            std::cout << "file " << filename << " not found.\n";
    }
    catch(const std::filesystem::filesystem_error& err) {
        // std::cout << "filesystem error: " << err.what() << '\n';
        throw std::system_error(err);
    }
}

int main(int argc, char* args[])
{
    if(argc==1)
    {
        throw std::invalid_argument("this program needs arguments, see the instructions.txt for more info");
    }
    else if(argc==4)
    {
        std::vector<std::string> env_var = apgn_env::grab(apgn::get_execpath()+"/env.txt",{
            "CHESS_ENGINE",
            "WORKER_THREADS",
            "ANALYSIS_DEPTH",
            "OPENING_MOVE_TO_SKIP"
        });

        std::string uci_converted_ = ".uci_converting", analyse_ = ".analysing";
        errorFileNotFound(args[1]);
        if(((args[3][0]!='W' && args[3][0]!='B') && args[3][0]!='A'))
        {
            throw std::invalid_argument("invalid 3rd  argument, choices are only A,W,B");
        }

        pgn_to_uci(args[1],uci_converted_);
        analyse_game(
            uci_converted_,
            analyse_,
            apgn::get_execpath()+"/bin/engines/"+env_var[CHESS_ENGINE],
            env_var[ANALYSIS_DEPTH].data(),
            env_var[WORKER_THREADS].data(),
            env_var[OPENING_MOVE_TO_SKIP].data(),
            args[3][0]
        );
        uci_to_pgn(analyse_,args[2]);
        deleteFile(uci_converted_);
        deleteFile(analyse_);
    }
    else
    {
        throw std::invalid_argument(
            "argument count is invalid!!!\n"
            "\t\tif your file path folders have names with spaces, enclose"
            "\t\tit between double quotes, so that the progam can read it properly");
    }
    // // ------------ load env variables -------------------

    // std::vector<std::string> env_var = apgn_env::grab("env.txt",{
    //     "CHESS_ENGINE",
    //     "WORKER_THREADS",
    //     "ANALYSIS_DEPTH",
    //     "OPENING_MOVE_TO_SKIP"
    // });

    // // ---------------------------------------------------
    // std::string pgn_file, pgn_output, uci_converted_ = ".uci_converting", analyse_ = ".analysing";

    // int threads = 1, depth = 12, skips = 0;

    // std::cout << "APGN : CHESS ANALYSER \n";

    // char c = 'c', side = 'n';

    // std::cout << "INPUT the character 'a' to analyse and 'h' for help\n";
    // std::cout<<"input : "; std::cin>>c;
    // std::cin.sync();
    // std::cin.clear();

    // if(c=='a')
    // {
    //     std::cout<<"TO START INPUT THE FOLLOWING PARAMETERS\n";

    //     std::cout<<"pgn's path with filename  : ";
    //     std::getline(std::cin,pgn_file);
    //     errorFileNotFound(pgn_file);

    //     std::cout<<"output path with filename : ";
    //     std::getline(std::cin,pgn_output);

    //     std::cout<<"input 'A' for both colors, 'W' for white only, and 'B' for black only\n";
    //     std::cout<<"which color to analyse? : ";
    //     std::cin>>side;

    //     if(((side!='W' && side!='B') && side!='A'))
    //     {
    //         throw std::invalid_argument("invalid choice, choices are only A,W,B");
    //     }

    //     pgn_to_uci(pgn_file,uci_converted_);
    //     analyse_game(
    //         uci_converted_,
    //         analyse_,
    //         "./bin/engines/"+env_var[CHESS_ENGINE],
    //         env_var[ANALYSIS_DEPTH].data(),
    //         env_var[WORKER_THREADS].data(),
    //         env_var[OPENING_MOVE_TO_SKIP].data(),
    //         side
    //     );
    //     uci_to_pgn(analyse_,pgn_output);
    //     deleteFile(uci_converted_);
    //     deleteFile(analyse_);
    // }
    // else if(c=='h')
    // {
    //     std::string help_info = "INFO:\nThis program analyses chess games, that is recorded in .PGN files\n"
    //         "this program can analyse, pgn file even it has multiple games in it, as long as they are valid pgn files\n"
    //         "pgn files have a certain structure, if your pgn file is not structured properly, you might need to\n"
    //         "fix it first before this program can analyse that pgn files without errors.\n\n\n"
    //         "WHAT ARE THE PARAMETERS NEEDED TO START ANALYZING?\n\n"
    //         "\tinorder to analyse pgn file, you first need to input certain parameters and these are\n\n"
            
    //         "\t - pgn's path with filename : \n"
    //         "\t\tex: C:/User/Documents/ChessGames/thePGN.pgn\n"
    //         "\t\t'C:/User/Documents/ChessGames/' is the file location\n"
    //         "\t\tand 'thePGN.pgn' is the chess game in pgn format.\n\n"

    //         "\t - output path with filename : \n"
    //         "\t\tthis will be the generated pgn file with comment analysis\n"
    //         "\t\tex: C:/User/Documents/ChessGames/analyzed.pgn\n"
    //         "\t\t'C:/User/Documents/ChessGames/analyzed' is the path where to save it\n"
    //         "\t\t'analyzed.pgn' is the filename to be saved\n\n"

    //         "\t - number of threads : \n"
    //         "\t\tthreads to be used by the chess engine...\n"
    //         "\t\ttypicaly a basic modern computer usually have 4 threads\n"
    //         "\t\tminimum value is 1, maximum value depends on the machine\n"
    //         "\t\tmore threads means faster analysis, though you should know\n"
    //         "\t\tyour computer's maximum threads first\n\n"

    //         "\t - engine depth level : \n"
    //         "\t\tminimum value is 1, maximum value depends on you\n"
    //         "\t\tthe bigger the depth, the stronger the analysis\n"
    //         "\t\talso bigger depth means longer analysis\n\n";

    //         "\t - opening moves to be skipped : \n"
    //         "\t\tthis are the moves you don't want to analyse at the beginning\n"
    //         "\t\tyou can set this parameter to zero,\n"
    //         "\t\tif you want to analyse even the openning moves\n\n\n"

    //         "INSTALLING NEW CHESS ENGINE\n\n"
    //         "\n\nby default apgn uses the 'stockfish10' chess engine\n"
    //         "but you can change the engine you want to use... here are the steps\n\n"

    //         "step 1 : \n"
    //         "\tfind and download a chess engine executable over the internet\n"
    //         "\tmake sure you download the right version for your architecture\n";
    //         "\tthere are x64, x86, and x86_64 architectures available\n";
    //         "\tchoose the one that is compatible with your device\n\n";

    //         "step 2 : \n"
    //         "\tafter getting a compatible chess engine for your device\n";
    //         "\tput the executable into the ./apgn/bin/engine folder\n"
    //         "\trename it into something easy to type and no spaces"

    //         "step 3 : \n"
    //         "\tafter putting it the the engine folder, you should specify the engine\n"
    //         "\tthat is being used in the env.txt, look for the 'CHESS_ENGINE=stockfish10'\n"
    //         "\tand change the 'stockfish10' into the name of the engine that you put in\n"
    //         "\tthe engine folder."

    //         "step 4 : your done and can use that engine now for analysis\n";

    //     std::cout<<help_info<<"\n";
    // }


    // #if defined(__linux__)

    // #elif defined(_WIN32)

    // #endif

    return 0;
}