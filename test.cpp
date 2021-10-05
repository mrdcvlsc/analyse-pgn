#include <iostream>
#include "board.hpp"

int main()
{
    apgn::board test;

    cyfre::display<char>(test.board_state);

    std::cout<<'\n';

    test.display_board();

    return 0;
}