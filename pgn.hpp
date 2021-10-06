#ifndef APGN_PGN_HPP
#define APGN_PGN_HPP

#include "cofun/strings/reader.hpp"

namespace apgn
{
    class PGN
    {
        public:

        std::string raw_pgn;
        std::vector<std::string> moves;

        PGN(std::string pgn_file="dev_ref/empty.pgn") : raw_pgn(read::all(pgn_file))
        {
            #ifdef DEBUG
            std::cout<<"phase 1\n";
            #endif

            size_t move_start = 0;
            for(size_t i=0; i<raw_pgn.size(); ++i)
            {
                if(raw_pgn[i]=='1' && raw_pgn[i+1]=='.' && raw_pgn[i+2]==' ')
                {
                    move_start = i+3;
                    break;
                }
            }

            #ifdef DEBUG
            std::cout<<"phase 2\n";
            #endif

            size_t iterator = move_start-1;
            while(raw_pgn[iterator]!='*' && iterator<raw_pgn.size())
            {
                std::string move = "";
                while(raw_pgn[++iterator]!=' ')
                {
                    move.push_back(raw_pgn[iterator]);
                }
                moves.push_back(move);

                #ifdef DEBUG
                std::cout<<move<<'|';
                #endif

                while(!isalpha(raw_pgn[iterator]))
                {
                    iterator++;
                }

                iterator--;
            }

            #ifdef DEBUG
            std::cout<<"phase 3\n";
            #endif
        }
    };
}

#endif