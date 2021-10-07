
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
#include "cofun/strings/removes.hpp"

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

        char& ct(char letter, char number)
        {
            size_t j = letter-'a';
            size_t i = '8'-number;

            return board_color(i,j);
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

        /// scans horizontally for attacking piece, returns an empty string if found nothing
        std::vector<std::string> scan_H(char letter, char number, char piece, char color)
        {
            std::vector<std::string> attacking_pieces;
            for(size_t i=0; i<8; ++i)
            {
                std::string attacking_piece = "";
                if(at(char('a'+i),number)==piece && ct(char('a'+i),number)==color && char('a'+i)!=letter)
                {
                    attacking_piece.push_back(char('a'+i));
                    attacking_piece.push_back(number);
                    attacking_pieces.push_back(attacking_piece);
                }
            }
            return attacking_pieces;
        }

        /// scans vertically for attacking piece, returns an empty string if found nothing
        std::vector<std::string> scan_V(char letter, char number, char piece, char color)
        {
            std::vector<std::string> attacking_pieces;
            for(size_t i=0; i<8; ++i)
            {
                std::string attacking_piece = "";
                if(at(letter,char('1'+i))==piece && ct(letter,char('1'+i))==color && char('1'+i)!=number)
                {
                    attacking_piece.push_back(letter);
                    attacking_piece.push_back(char('1'+i));
                    attacking_pieces.push_back(attacking_piece);
                }
            }
            return attacking_pieces;
        }

        /// scan diagonals
        std::string scan_D(char letter, char number, char piece, char color)
        {
            return "undefined";
        }

        /// scan L shape squares
        std::string scan_L(char letter, char number, char piece, char color)
        {
            return "undefined";
        }

        void move(const std::string& move)
        {
            std::cout<<"move : "<<move<<"\n";

            char piece = piece_move(move);

            /// pawn move
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
                    ct(move[2],move[3]) = turn;

                    if(turn=='W')
                    {
                        at(move[0],char(move[3]-1)) = ' ';
                        ct(move[0],char(move[3]-1)) = ' ';

                        if(enpasant)
                        {
                            at(move[2],char(move[3]-1)) = ' ';
                            ct(move[2],char(move[3]-1)) = ' ';
                        }

                        turn = 'B';
                    }
                    else
                    {
                        at(move[0],char(move[3]+1)) = ' ';
                        ct(move[0],char(move[3]+1)) = ' ';

                        if(enpasant)
                        {
                            at(move[2],char(move[3]+1)) = ' ';
                            ct(move[2],char(move[3]+1)) = ' ';
                        }

                        turn = 'W';
                    }
                }
                else
                {
                    // forward pawn move
                    at(move[0],move[1]) = piece;
                    ct(move[0],move[1]) = turn;

                    if(turn=='W')
                    {
                        for(size_t i=1; i<3; ++i)
                        {
                            if(at(move[0],char(move[1]-i))=='P')
                            {
                                at(move[0],char(move[1]-i)) = ' ';
                                ct(move[0],char(move[1]-i)) = ' ';
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
                                ct(move[0],char(move[1]+i)) = ' ';
                                break;
                            }
                        }

                        turn = 'W';
                    }
                }
            }
            else
            {
                std::string Move = removes::match(move,"x");

                /// the peice is aligned to the same piece
                bool aligned = Move.size()==4 ? true : false;

                // if the number is present, then the piece is aligned with the other same piece in the letter line
                //      ex: R2f4 means that the rook came from Rf2

                // if the letter is present, then the piece is aligned with the other same piece in the number line
                //      ex: Rbe4 means that the rook came from Rb4

                switch(piece)
                {
                    case 'R':
                        if(!aligned)
                        {
                            std::vector<std::string> rook_pos = scan_H(move[1],move[2],'R',turn);
                            if(rook_pos.empty())
                            {
                                rook_pos = scan_V(move[1],move[2],'R',turn);
                            }

                            // remove old position
                            std::string old_pos = rook_pos[0];
                            at(old_pos[0],old_pos[1]) = ' ';
                            ct(old_pos[0],old_pos[1]) = ' ';

                            // place to new position
                            at(move[1],move[2]) = 'R';
                            ct(move[1],move[2]) = turn;
                        }
                        else
                        {
                            char old_lpos;
                            char old_npos;

                            if(isdigit(move[1]))
                            {
                                old_lpos = move[2];
                                old_npos = move[1];
                            }
                            else
                            {
                                old_lpos = move[1];
                                old_npos = move[3];
                            }

                            // remove old position
                            at(old_lpos, old_npos) = ' ';
                            ct(old_lpos, old_npos) = ' ';

                            // place to new position
                            at(move[2],move[3]) = 'R';
                            ct(move[2],move[3]) = turn;
                        }
                        break;
                    case 'N':
                        break;
                    case 'B':
                        break;
                    case 'K':
                        break;
                    case 'Q':
                        break;
                    default:
                        throw std::logic_error("WHILE MOVING A PIECE SOMETHING WENT WRONG, AND IDK WTF IS IT!");
                }
                turn = turn=='W' ? 'B' : 'W';
            }
        }

        void display_board_state()
        {
            for(size_t i=0; i<8; ++i)
            {
                std::cout<<"+---+---+---+---+---+---+---+---+\t+---+---+---+---+---+---+---+---+\n";
                for(size_t j=0; j<8; ++j)
                {                    
                    std::cout<<"| "<<board_state(i,j)<<" ";
                    if(j==7)
                    {
                        std::cout<<'|';
                    }
                }
                std::cout<<" "<<8-i<<"\t";
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
            std::cout<<"+---+---+---+---+---+---+---+---+\t+---+---+---+---+---+---+---+---+\n";
            std::cout<<"  a   b   c   d   e   f   g   h  \t===== C   O   L   O   R   S =====\n";
        }
    };

}

#endif