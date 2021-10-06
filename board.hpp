
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
#include <cctype>
#include <string>

#include "cyfre/cyfre.hpp"
#include "cofun/strings/reader.hpp"

namespace apgn
{
    const std::string coordinates = "abcdefgh";
    size_t coord(char c)
    {
        return c-'a';
    }

    const std::string PIECES = "RNBKQ";

    class board
    {
        public:

        cyfre::mat<char> board_state;
        cyfre::mat<char> board_color;
        std::vector<std::string> moves;

        char turn;

        char& at(char letter, char number)
        {
            size_t j = letter-'a';
            size_t i = '8'-number;

            return board_state(i,j);
        }

        board(char turn='W') : turn(turn)
        {
            board_state = cyfre::mat<char>(8,8,' ');
            board_color = cyfre::mat<char>(8,8,' ');

            // set peice colors
            for(size_t i=0; i<2; ++i)
            {
                for(size_t j=0; j<8; ++j)
                {
                    board_color(i,j) = 'B';
                }
            }

            for(size_t i=6; i<8; ++i)
            {
                for(size_t j=0; j<8; ++j)
                {
                    board_color(i,j) = 'W';
                }
            }

            // set peice position
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

        char piece_move(const std::string& move)
        {
            for(size_t i=0; i<PIECES.size(); ++i)
            {
                if(move[0]==PIECES[i])
                {
                    return PIECES[i];
                }
            }
            return 'P';
        }

        void move(const std::string& move)
        {
            std::cout<<"move : "<<move<<"\n";

            char piece = piece_move(move);

            if(piece=='P')
            {
                if(move.size()>2)
                {
                    bool enpasant = false;
                    if(at(move[2],move[3])==' ')
                    {
                        enpasant = true;
                    }

                    // capture pawn move
                    at(move[2],move[3]) = piece;

                    if(turn=='W')
                    {
                        at(move[0],char(move[3]-1)) = ' ';

                        if(enpasant)
                        {
                            at(move[2],char(move[3]-1)) = ' ';
                        }

                        turn = 'B';
                    }
                    else
                    {
                        at(move[0],char(move[3]+1)) = ' ';

                        if(enpasant)
                        {
                            at(move[2],char(move[3]+1)) = ' ';
                        }

                        turn = 'W';
                    }
                }
                else
                {
                    // forward pawn move
                    at(move[0],move[1]) = piece;

                    if(turn=='W')
                    {
                        for(size_t i=1; i<3; ++i)
                        {
                            if(at(move[0],char(move[1]-i))=='P')
                            {
                                at(move[0],char(move[1]-i)) = ' ';
                                break;
                            }
                        }

                        turn = 'B';
                    }
                    else
                    {
                        for(size_t i=1; i<3; ++i)
                        {
                            if(at(move[0],char(move[1]+i))=='P')
                            {
                                at(move[0],char(move[1]+i)) = ' ';
                                break;
                            }
                        }

                        turn = 'W';
                    }
                }
            }
            else
            {

            }
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
                std::cout<<" "<<8-i<<"\n";
            }
            std::cout<<"+---+---+---+---+---+---+---+---+\n";
            std::cout<<"  a   b   c   d   e   f   g   h  \n";
        }

        void display_color()
        {
            for(size_t i=0; i<8; ++i)
            {
                std::cout<<"+---+---+---+---+---+---+---+---+\n";
                for(size_t j=0; j<8; ++j)
                {                    
                    std::cout<<"| "<<board_color(i,j)<<" ";
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