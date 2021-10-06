#include <iostream>
#include "board.hpp"

int main()
{
    apgn::board test;

    std::cout<<"readed\n";

    std::cout<<"board peicees = \n\n";
    test.display_board();

    test.move("e4");

    std::cout<<"board peicees = \n\n";
    test.display_board();

    test.move("e5");

    std::cout<<"board peicees = \n\n";
    test.display_board();

    test.move("a3");

    std::cout<<"board peicees = \n\n";
    test.display_board();

    test.move("h6");

    std::cout<<"board peicees = \n\n";
    test.display_board();

    // std::cout<<"board colors = \n\n";
    // test.display_color();

    return 0;
}