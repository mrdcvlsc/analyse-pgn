#include <iostream>
#include <vector>
#include <exception>

#include <string.h>

#include "convert.hpp"
#include "envgrabber.hpp"
#include "check.hpp"
#include "path.hpp"


#define CHESS_ENGINE 0
#define WORKER_THREADS 1
#define ANALYSIS_DEPTH 2
#define OPENING_MOVE_TO_SKIP 3

int main(int argc, char* args[])
{
    // display help
    if(argc==2)
    {
        if(strcmp(args[1],"--help")==0 || strcmp(args[1],"-h")==0)
        {
            std::cout<<"Arguments :\n\n";

            std::cout<<"\tTo display this message, input the command : \n\n";
            std::cout<<"\t\tapgn --help\n";
            std::cout<<"\tor\n";
            std::cout<<"\t\tapgn -h\n\n\n";

            std::cout<<
                "\tTo analyse a pgn file copy the pgn's filename with it's path location\n"
                "\tand paste it after apgn in the command line (or drag the file to the\n"
                "\tterminal to paste it's path) followed by either one of the three letters\n"
                "\t'A','W', and 'B'. each letter coresponds the side you want to analyse\n"
                "\t'W' for white, 'B' for black, and 'A' for both.\n"
                "\n\tSample Analyse Command :\n\n"
                "\t\tapgn "+apgn::get_execpath()+"/pgn_samples/first.pgn W\n\n"
                "\tthe command above will analyse the pgn file 'first.pgn' for player white\n"
                "\tto avoid errors make sure the your PGN file has a lowercase extension '.pgn'\n\n\n"
                "\tTo adjust the settings values that the engine will use, open\n"
                "\tthe file "+apgn::get_execpath()+"env.txt file and tweek the values\n";
        }
        else
        {
            std::cout<< "apgn: analyse-pgn\n\ta simple chess game analyser\n"
                        "\t-h or --help for more info\n"
                        "\n\n\t\t\tLicence: GPLv3\n";
        }
    }
    // if proper arguments are received
    else if(argc==3)
    {
        std::string out_pgn, format = ".pgn", statsFileName, statWriteCache="", destinationPath;
        std::string uci_converted_ = apgn::get_execpath()+"/.analyzing_moves", analyse_ = apgn::get_execpath()+"/.analyse";

        // checks the first argument if it has a valid .pgn file extension
        if(!apgn::match_last(args[1],format))
        {
            // throw error if it doesn't
            throw std::invalid_argument("please provided a pgn file with an extension '.pgn' all lowercase");
        }
        else
        {
            // process the string if it has .pgn in it

            // removes the .pgn substring in the first argument and store the resulting as out_pgn
            out_pgn = apgn::removeFormat(args[1],format);

            // get the destination of the output pgn by removing only the name of input pgn
            destinationPath = apgn::removeFilename(out_pgn); // output pgn will be saved on the same location as the input pgn
            
            // add the string 'Analyzed' to the input pgn's name and make this as the output pgn's filename 
            statsFileName = out_pgn;
            out_pgn+="Analyzed.pgn";

            // this will be the filename for the output statisics text file of the input pgn
            // this is different from the analysed pgn
            statsFileName+="AnalyzedStats.txt";
        }

        // In total there are two output when analysing a pgn game

        // 1. THE OUTPUT PGN - this is a pgn file that contains the same moves as the input pgn file
        //                     but, this also additional comments for each analyzed moves, the comment
        //                     will tell if your move is a good move, blunder, excellent and etc.

        // 2. THE STATS TXT - this is a text file that will contain the statistics of the analyzed moves
        //                    here will see how many good move, blunders, mistakes and etc. you have made
        //                    this will also contain an accuracy for the total moves you analyzed


        // grabs the value in the env.txt files, and if the engines exist
        std::vector<std::string> env_var = apgn_env::grab(apgn::get_execpath()+"/env.txt",{
            #if defined(__linux__)
            "CHESS_ENGINE_LINUX",
            #elif defined(_WIN32)
            "CHESS_ENGINE_WINDOWS",
            #endif
            "WORKER_THREADS",
            "ANALYSIS_DEPTH",
            "OPENING_MOVE_TO_SKIP"
        });
        apgn::errorFileNotFound(apgn::get_execpath()+"/bin/engines/"+env_var[CHESS_ENGINE]);
        
        // check if the input pgn exist
        apgn::errorFileNotFound(args[1]);

        // checks if the 3rd argument passed to the program is valid
        if(((args[2][0]!='W' && args[2][0]!='B') && args[2][0]!='A'))
        {
            throw std::invalid_argument("invalid 3rd  argument, choices are only A,W,B");
        }

        // CONVERSION OF PGN TO UCI PGN
        pgn_to_uci(args[1],uci_converted_);
        
        // ANALYSIS OF THE CONVERTED UCI PGN
        std::cout<<"analysing game(s)...\n\n";
        int depthValue = stoi(env_var[ANALYSIS_DEPTH]);
        if(depthValue>12)
        {
            std::cout<<"NOTE: env.txt's 'ANALYSIS_DEPTH' value is greater than 12\n";
            std::cout<<"because of this, analysis might take a few minutes\n";
        }
        analyse_game(
            uci_converted_,
            analyse_,
            apgn::get_execpath()+"/bin/engines/"+env_var[CHESS_ENGINE],
            env_var[ANALYSIS_DEPTH].data(),
            env_var[WORKER_THREADS].data(),
            env_var[OPENING_MOVE_TO_SKIP].data(),
            args[2][0]
        );

        // CONVERT BACK THE ANALYZED UCI PGN INTO PROPER PGN NOTATION
        uci_to_pgn(analyse_,out_pgn);
        
        // WRITE THE STATISTIC TXT FILE INTO THE SAME DESTINATION OF THE OUTPUT PGN
        std::string stats_content = "";
        std::ifstream stats_file_cache(apgn::get_execpath()+"/.analyzing_moves_stats");
        if(stats_file_cache.is_open())
        {
            std::string lineBuffer;
            while(getline(stats_file_cache,lineBuffer))
            {
                stats_content.append(lineBuffer+"\n");
            }
            stats_file_cache.close();
            apgn::deleteFile(apgn::get_execpath()+"/.analyzing_moves_stats");
        }
        else
        {
            throw std::runtime_error(
                "the file '" + apgn::get_execpath()+"/.analyzing_moves_stats' was not found"
            );
        }
        stats_content.append("\n\n");
        std::ofstream outfile;
        outfile.open(statsFileName,std::ios_base::trunc);
        outfile<<stats_content;
        outfile.close();

        // CLEAN UP THE GENERATED RESOURCES IN THE BACKGROUND 
        apgn::deleteFile(uci_converted_);
        apgn::deleteFile(analyse_);
    }
    // if arguments are not meet
    else
    {
        std::cout<< "invalid arguments, for more information about this program\n"
                    "type the command : \n\napgn -h\n\nor\n\napgn --help\n\n"
                    "to display informations on how to use this program, or \n"
                    "you can visit my github https://github.com/mrdcvlsc/analyse-pgn\n";
    }

    return 0;
}