#include <iostream>
#include "pgn.hpp"

int main()
{
    apgn::PGN read_game("dev_ref/clean.pgn");

    std::cout<<"PGN CONTENT : \n"<<read_game.raw_pgn<<"\n\n";

    std::cout<<"moves: ";

    for(auto e: read_game.moves)
    {
        std::cout<<e<<'\n';
    }

    std::cout<<"\n\nEND OF PROGRAM";
    std::cin.get();
    return 0;
}