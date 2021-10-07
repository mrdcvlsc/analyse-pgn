#include <iostream>
#include "board.hpp"

int main()
{
    apgn::board test;

    test.display_board_state();
    std::cout<<"\n";

    test.move("Rad1");
    test.display_board_state();
    std::cout<<"\n";

    test.move("a5");
    test.display_board_state();
    std::cout<<"\n";

    test.move("Rhd1");
    test.display_board_state();
    std::cout<<"\n";

    return 0;
}