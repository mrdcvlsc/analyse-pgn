#ifndef APGN_CONVERT_HPP
#define APGN_CONVERT_HPP

#include <iostream>
#include <filesystem>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <fstream>

int run_subprog(const char* program, char *const args[])
{
    int status;

    if(!std::filesystem::exists(program))
    {
        throw std::logic_error("'"+std::string(program)+"' file not found");
    }

    int pid = fork();

    if(pid==-1) throw std::runtime_error("error forking subprocess");

    if(pid==0)
    {
        execv(program,args);
    }
    else
    {
        waitpid(pid, &status, 0);
    }

    return status;
}


// PGN to UCI
// ./pgn-extract -Wuci --output converted.pgn to_convert.pgn

// UCI to PGN
// ./pgn-extract -WsanPNBRQK --output converted.pgn to_convert.pgn

void uci_to_pgn(const std::string& input, const std::string output)
{
    // input = 
    char *const args[] = {"pgn-extract","-WsanPNBRQK","--output",(char* const)output.c_str(),(char* const)input.c_str(),NULL};
    run_subprog("./bin/pgn-extract",args);
}

void pgn_to_uci(const std::string& input, const std::string output)
{
    char *const args[] = {"pgn-extract","-Wuci","--output",(char* const)output.c_str(),(char* const)input.c_str(),NULL};
    run_subprog("./bin/pgn-extract",args);
}

namespace COLOR
{
    const char ALL = 'A';
    const char WHITE = 'W';
    const char BLACK = 'B';
}

void analyse_game(
    const std::string& input,
    const std::string& output,
    const std::string& engine,
    char* search_depth,
    char* threads,
    char* opening_move_skips,
    char apgn_COLOR
)   
{
    char *const args[] = {
        "analyse",
        "--engine",(char* const)engine.c_str(),
        "--searchdepth",search_depth,
        "--bookdepth",opening_move_skips,
        "--setoption","Threads",threads,
        "--annotatePGN",(char* const)input.c_str(),
        NULL
    };

    char *const args_white[] = {
        "analyse",
        "--engine",(char* const)engine.c_str(),
        "--whiteonly",
        "--searchdepth",search_depth,
        "--bookdepth",opening_move_skips,
        "--setoption","Threads",threads,
        "--annotatePGN",(char* const)input.c_str(),
        NULL
    };

    char *const args_black[] = {
        "analyse",
        "--engine",(char* const)engine.c_str(),
        "--blackonly",
        "--searchdepth",search_depth,
        "--bookdepth",opening_move_skips,
        "--setoption","Threads",threads,
        "--annotatePGN",(char* const)input.c_str(),
        NULL
    };

    std::string result_pgn = "";
    char std_output[4096+1];
    memset(std_output,0,4096);

    int reader[2];
    if(pipe(reader)==-1) std::runtime_error("error creating pipe for stdout reading in analyse_game()");

    int pid = fork();

    if(pid==-1) std::runtime_error("error forking parent process, unable to create a child process in analyse_game()");

    if(pid==0)
    {
        dup2(reader[1], STDOUT_FILENO);
        close(reader[0]);
        switch (apgn_COLOR)
        {
        case COLOR::ALL:
            execv("./bin/analyse",args);
            break;
        case COLOR::WHITE:
            execv("./bin/analyse",args_white);
            break;
        case COLOR::BLACK:
            execv("./bin/analyse",args_black);
            break;
        default:
            throw std::runtime_error("something happend - error in convert.hpp");
            break;
        }
        close(reader[1]);
    }
    else
    {
        close(reader[1]);
        int nbytes = 0;
        while(read(reader[0], std_output, sizeof(std_output)) != 0)
        {
            result_pgn+=std_output;
            memset(std_output,0,4096);
        }
        close(reader[0]);
        wait(NULL);
    }

    // check if file exist
    std::ifstream readf;
    readf.open(output);

    if(readf.fail()){
        readf.close();

        std::ofstream file;
        file.open(output);
        file.close();
    }

    std::ofstream outfile;
    outfile.open(output,std::ios_base::trunc);
    outfile<<result_pgn;
    outfile.close();

    ///analyse --engine ./stockfish14 --blackonly --searchdepth 20 --bookdepth 0 --setoption Threads 4 --annotatePGN uci_mygame.pgn > analyzed_uci_mygame.pgn
}

#endif