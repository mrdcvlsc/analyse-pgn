#include "interpret.hpp"

using namespace std;

namespace interpret
{
    std::ostringstream cpPrec;
    
    void initializeOStream()
    {
        cpPrec << fixed;
        cpPrec << setprecision(2);
    }

    std::string displayCP(int cpEval)
    {
        std::string strForm = "";

        double centipawns = cpEval;

        if (cpEval>0) strForm.append("(+");
        else          strForm.append("(");

        centipawns/=100;
        cpPrec << centipawns;

        strForm.append(cpPrec.str()+")");
        
        cpPrec.str("");
        cpPrec.clear();

        return strForm;
    }

    void playedMove(int played_move, int best_move)
    {
        int interpretation = played_move-best_move;

        if(played_move>650)
        {
            if     (interpretation >  -75) std::cout<<" winning : move is in top cantidate ";
            else if(interpretation > -150) std::cout<<" winning : move has medium advantage ";
            else if(interpretation > -300) std::cout<<" winning : move is slightly passive ";
            else if(interpretation > -450) std::cout<<" winning but lost small advantage ";
            else if(interpretation > -610) std::cout<<" winning but lost huge advantage ";
            else                           std::cout<<" winning but you missed all the good moves ";
        }
        else if(played_move<-650)
        {
            if     (interpretation > -100)  std::cout<<" losing : top move but losing already ";
            else if(interpretation > -300)  std::cout<<" losing : not the best move ";
            else if(interpretation > -600)  std::cout<<" losing : this move speeds up your demise";
            else if(interpretation > -900)  std::cout<<" losing : you should have resigned at this point ";
            else if(interpretation > -9999) std::cout<<" losing : why are you still here? just to suffer? ";
            else                            std::cout<<" losing : either you guys are complete noobs or, he's just playing "
                                                     <<" with his food and you're being too stubborn ";
        }
        else
        {
            if(played_move==best_move) std::cout<<" brilliant move ";
            else if(interpretation >  -10) std::cout<<" excellent move ";
            else if(interpretation >  -20) std::cout<<" good move ";
            else if(interpretation >  -60) std::cout<<" okay move ";
            else if(interpretation > -160) std::cout<<" inaccurate move ";
            else if(interpretation > -220) std::cout<<" flawd move ";
            else if(interpretation > -300) std::cout<<" mistake ";
            else
            {
                if(played_move<0) std::cout<<" blunder ";
                else              std::cout<<" missed win or advantage ";
            }
        }

        cout << displayCP(played_move) ;
    }
}