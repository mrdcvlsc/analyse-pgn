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
    if(argc==2)
    {
        if(strcmp(args[1],"--help")==0 || strcmp(args[1],"-h")==0)
        {
            std::cout<<"Arguments :\n\n";

            std::cout<<"\tTo display this message, input the command : \n\n";
            std::cout<<"\t\tapgn --help\n";
            std::cout<<"\tor\n";
            std::cout<<"\t\tapgn -h\n\n\n";

            std::cout<<"\tTo analyse a pgn file copy the pgn's filename with it's path location\n";
            std::cout<<"\tand paste it after apgn in the command line (or drag the file to the terminal to paste it's path)\n";
            std::cout<<"\tfollowed by either one of the three letters 'A','W', and 'B'. each letter coresponds the\n";
            std::cout<<"\tside you want to analyse, 'W' for white, 'B' for black, and 'A' for both, below is a";
            std::cout<<"\n\tSample Analyse Command :\n\n";
            std::cout<<"\t\tapgn "+apgn::get_execpath()+"/pgn_samples/first.pgn W\n\n";
            std::cout<<"\tthe command above will analyse the pgn file 'first.pgn' for player white\n";
            std::cout<<"\tmake sure that your pgn file has lower case extension name '.pgn' so it won't throw errors\n\n\n";

            std::cout<<"\tTo adjust the settings that the engine will use, open\n\tthe file "+apgn::get_execpath()+"env.txt file\n";
        }
        else
        {
            std::cout<< "apgn: analyse-pgn\n\ta simple chess game analyser\n"
                        "\t-h or --help for more info\n"
                        "\n\n\t\t\tLicence: GPLv3\n";
        }
    }
    else if(argc==3)
    {
        std::string out_pgn, format = ".pgn";

        if(!apgn::match_last(args[1],format))
        {
            throw std::invalid_argument("please provided a pgn file with an extension '.pgn' all lowercase");
        }
        else
        {
            out_pgn = apgn::removeFormat(args[1],format);
            out_pgn+="Analyzed.pgn";
        }

        std::vector<std::string> env_var = apgn_env::grab(apgn::get_execpath()+"/env.txt",{
            "CHESS_ENGINE",
            "WORKER_THREADS",
            "ANALYSIS_DEPTH",
            "OPENING_MOVE_TO_SKIP"
        });

        apgn::errorFileNotFound(apgn::get_execpath()+"/bin/engines/"+env_var[CHESS_ENGINE]);

        std::string uci_converted_ = apgn::get_execpath()+"/.uci_convert", analyse_ = apgn::get_execpath()+"/.analyse";

        apgn::errorFileNotFound(args[1]);

        if(((args[2][0]!='W' && args[2][0]!='B') && args[2][0]!='A'))
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
            args[2][0]
        );

        uci_to_pgn(analyse_,out_pgn);

        apgn::deleteFile(uci_converted_);

        apgn::deleteFile(analyse_);
    }
    else
    {
        std::cout<< "apgn: analyse-pgn\n\ta simple chess game analyser\n"
                        "\t-h or --help for more info\n"
                        "\n\n\t\t\tLicence: GPLv3\n";
    }

    return 0;
}