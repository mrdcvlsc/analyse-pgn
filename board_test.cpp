#include <iostream>
#include "board.hpp"

int main()
{
    apgn::board test;

    std::cout<<"readed\n";

    std::cout<<"board peicees = \n\n";
    test.display_board();

    test.move("d4");
    std::cout<<"board peicees = \n\n";
    test.display_board();

    test.move("h5");
    std::cout<<"board peicees = \n\n";
    test.display_board();

    test.move("d5");
    std::cout<<"board peicees = \n\n";
    test.display_board();

    test.move("e5");
    std::cout<<"board peicees = \n\n";
    test.display_board();

    test.move("dxe6");
    std::cout<<"board peicees = \n\n";
    test.display_board();

    test.move("fxe6");
    std::cout<<"board peicees = \n\n";
    test.display_board();

    return 0;
}