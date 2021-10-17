#include <iostream>
#include "convert.hpp"
#include <sys/wait.h>
#include <filesystem>
#include <vector>
#include "envgrabber.hpp"


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

int main()
{
    std::vector<std::string> env_variables = apgn_env::grab("env.txt",{
        "CHESS_ENGINE",
        "WORKER_THREADS",
        "ANALYSIS_DEPTH",
        "OPENING_MOVE_TO_SKIP"
    });

    std::string
        file1 = "./test/first.pgn",
        file2 = "./test/second.pgn",
        file3 = "./test/third.pgn",
        file4 = "./test/fourth.pgn",
        engine = "./bin/engines/"+env_variables[0],
        threads = env_variables[1],
        depth = env_variables[2],
        skips = env_variables[3];

    std::cout<<"START : PARENT\n";

    char *const args[] = {env_variables[0].data(),NULL};

    std::cout<<"converting games\n";
    pgn_to_uci(file1,file2);

    analyse_game(
        file2,
        file3,
        engine,
        depth.data(),threads.data(),skips.data(),COLOR::BLACK
    );

    deleteFile(file2);

    std::cout<<"\nprocessing analysis\n";
    uci_to_pgn(file3,file4);

    deleteFile(file3);

    std::cout<<"PARENT DONE WITH CHILD\n";

    return 0;
}