#include <iostream>
#include "convert.hpp"
#include <sys/wait.h>
#include <filesystem>
#include <vector>

int main() {

    std::cout<<"START : PARENT\n";

    char *const args[] = {"stockfish10_linux",NULL};

    std::cout<<"converting games\n";
    pgn_to_uci("./test/first.pgn","./test/second.pgn");
    analyse_game(
        "./test/second.pgn",
        "./test/third.pgn",
        "./bin/stockfish10_linux",
        "16","4","0",COLOR::WHITE);
    std::cout<<"\nprocessing analysis\n";
    uci_to_pgn("./test/third.pgn","./test/fourth.pgn");

    std::cout<<"PARENT DONE WITH CHILD\n";

    return 0;
}