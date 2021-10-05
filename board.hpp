
// board state class

// + : read_pgn_constructor()
// + : next_move()
// + : prev_move()
// + : display()

// + : game_info [vec|pair]
// + : uci_moves [vec]
// + : piece_dict
// + : - counts
// + : - positions

#ifndef APGN_PP_HPP
#define APGN_PP_HPP

#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include "cyfre/cyfre.hpp"

namespace apgn
{
    class board
    {
        public:

        cyfre::mat<char> board_state;

        board(std::string pgn_file="none")
        {
            board_state = cyfre::mat<char>(8,8,' ');
            for(size_t i=0; i<8; ++i)
            {
                board_state(1,i) = 'P';
                board_state(6,i) = 'P';
            }

            board_state(0,0) = board_state(0,7) = board_state(7,0) = board_state(7,7) = 'R'; // Rooks
            board_state(0,1) = board_state(0,6) = board_state(7,1) = board_state(7,6) = 'N'; // Knight
            board_state(0,2) = board_state(0,5) = board_state(7,2) = board_state(7,5) = 'B'; // Bishop
            board_state(0,3) = board_state(7,3) = 'Q'; // Queen
            board_state(0,4) = board_state(7,4) = 'K'; // King
        }

        void display_board()
        {
            for(size_t i=0; i<8; ++i)
            {
                std::cout<<"+---+---+---+---+---+---+---+---+\n";
                for(size_t j=0; j<8; ++j)
                {                    
                    std::cout<<"| "<<board_state(i,j)<<" ";
                    if(j==7)
                    {
                        std::cout<<'|';
                    }
                }
                std::cout<<'\n';
            }
            std::cout<<"+---+---+---+---+---+---+---+---+\n";
        }
    };

}

#endif