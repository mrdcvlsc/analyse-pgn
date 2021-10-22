#include <map>
#include <cmath>
#include "interpret.hpp"

using namespace std;

namespace interpret
{
    short brilliantMoves[2];
    short excellentMove[2];
    short goodMove[2];
    short inaccurateMove[2];
    short badMove[2];
    short mistake[2];
    short blunder[2];
    short missedWin[2];
    short winningMoves[2];
    short losingMoves[2];
    long long playedEval[2];
    long long bestEval[2];

    ostringstream cpPrec;
    
    void initializeOStream()
    {
        cpPrec << fixed;
        cpPrec << setprecision(2);
    }

    void clearStats()
    {
        brilliantMoves[0] = brilliantMoves[1] = 0;
        excellentMove[0] = excellentMove[1] = 0;
        goodMove[0] = goodMove[1] = 0;
        inaccurateMove[0] = inaccurateMove[1] = 0;
        badMove[0] = badMove[1] = 0;
        mistake[0] = mistake[1] = 0;
        blunder[0] = blunder[1] = 0;
        missedWin[0] = missedWin[1] = 0;
        winningMoves[0] = winningMoves[1] = 0;
        losingMoves[0] = losingMoves[1] = 0;
        playedEval[0] = playedEval[1] = 0;
        bestEval[0] = bestEval[1] = 0;
    }

    void recordStats(const string& saveFile, bool color, size_t gameNumber)
    {
        long long upper = playedEval[color], lower = bestEval[color];
        
        if(upper<0 && lower<0)
        {
            upper = abs(upper);
            lower = abs(lower);
            swap(upper,lower);
        }
        else if(upper<0 && lower>=0)
        {
            cout << "\n\n\nERROR : AN EDGE CASE OCCUR IN GETTING THE ACCURACY, WHERE TOTAL PLAYED_MOVE EVAL \n";
            cout << "        IS A NEGATIVE VALUE, AND THE BEST EVAL IS A POSITIVE VALUE\n\n";
            throw std::runtime_error("detected an edge case");
        }

        long double accuracy = ((long double)upper/(long double)lower)*100L;
        
        string ColorLabel = (color ? "W" : "B");
        string statCounts="- - - - - - - - - - - - - - - - - - - - - - - - - - - \n"
                          "Game# : " + to_string(gameNumber) + "\n"
                          "COLOR : " + ColorLabel + "\n";
        statCounts.append(" - - - - - - - - Analyzed Move Counts - - - - - - - - \n");
        statCounts.append( "\tBriliant Moves   - " + to_string(brilliantMoves[color]) + "\n");
        statCounts.append( "\tExcellent Moves  - " + to_string(excellentMove[color]) + "\n");
        statCounts.append( "\tGood Moves       - " + to_string(goodMove[color]) + "\n");
        statCounts.append( "\tinaccurate Moves - " + to_string(inaccurateMove[color]) + "\n");
        statCounts.append( "\tBad Moves        - " + to_string(badMove[color]) + "\n");
        statCounts.append( "\tMistakes         - " + to_string(mistake[color]) + "\n");
        statCounts.append( "\tBlunders         - " + to_string(blunder[color]) + "\n");
        statCounts.append( "\tAlready Winning  - " + to_string(winningMoves[color]) + "\n");
        statCounts.append( "\tAlready Losing   - " + to_string(losingMoves[color]) + "\n");
        statCounts.append( "\tTOTAL : " + to_string(
          ( brilliantMoves[color] + excellentMove[color] + goodMove[color] + inaccurateMove[color] +
            badMove[color] + mistake[color] + blunder[color] + winningMoves[color] +
            losingMoves[color]))+"\n"
          );

        #ifndef PRODUCTION
        statCounts.append("\nPlayed Move Total Centipawns = " + to_string(playedEval[color]));
        statCounts.append("\nBest Move Total Centipawns = " + to_string(bestEval[color]));
        statCounts.append("\nmin|max = "+to_string(upper)+"|" + to_string(lower));
        #endif
        
        cpPrec.str("");
        cpPrec.clear();
        cpPrec << accuracy;
        statCounts.append("\n\tAccuracy : "+cpPrec.str()+"%\n\n");
        cpPrec.str("");
        cpPrec.clear();

        ofstream outfile;
        outfile.open(saveFile+"_stats",ios_base::out | ios_base::app);
        outfile<<"==============================================================\n";
        outfile<<statCounts;
        outfile.close();
    }

    string displayCP(int cpEval)
    {
        string strForm = "";

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

    void playedMove(int played_move, int best_move, bool color)
    {
        int interpretation = played_move-best_move;

        playedEval[color] += played_move;
        bestEval[color] += best_move;

        if(played_move>650)
        {
            winningMoves[color]++;
            if     (interpretation >  -75) cout<<" winning : move is in top cantidate ";
            else if(interpretation > -150) cout<<" winning : move has medium advantage ";
            else if(interpretation > -300) cout<<" winning : move is slightly passive ";
            else if(interpretation > -450) cout<<" winning but lost small advantage ";
            else if(interpretation > -610) cout<<" winning but lost huge advantage ";
            else                           cout<<" winning but you missed all the good moves ";
        }
        else if(played_move<-650)
        {
            losingMoves[color]++;
            if     (interpretation > -100)  cout<<" losing : top move but losing already ";
            else if(interpretation > -300)  cout<<" losing : not the best move ";
            else if(interpretation > -600)  cout<<" losing : this move speeds up your demise";
            else if(interpretation > -900)  cout<<" losing : you should have resigned at this point ";
            else if(interpretation > -9999) cout<<" losing : why are you still here? just to suffer? ";
            else                            cout<<" losing : either you guys are complete noobs or, he's just playing "
                                                     <<" with his food and you're being too stubborn ";
        }
        else
        {
            if(played_move==best_move)
            {
                brilliantMoves[color]++;
                cout<<" brilliant move ";
            }
            else if(interpretation >  -10)
            {
                excellentMove[color]++;
                cout<<" excellent move ";
            }
            else if(interpretation >  -60)
            {
                goodMove[color]++;
                cout<<" good move ";
            }
            else if(interpretation > -110)
            {
                inaccurateMove[color]++;
                cout<<" inaccurate move ";
            }
            else if(interpretation > -160)
            {
                badMove[color]++;
                cout<<" bad move ";
            }
            else if(interpretation > -210)
            {
                mistake[color]++;
                cout<<" mistake ";
            }
            else
            {
                if(played_move<0)
                {
                    blunder[color]++;
                    cout<<" blunder ";
                }
                else
                {
                    missedWin[color]++;
                    cout<<" missed win or advantage ";
                }
            }
        }

        cout << displayCP(played_move) ;
    }

    std::string get_execpath()
    {
        #if defined(__linux__)
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        const char *path;
        if (count != -1) {
            path = dirname(result);
        }
        return path;
        #elif defined(_WIN32)
        WCHAR path[MAX_PATH];
        GetModuleFileNameW(NULL, path, MAX_PATH);

        std::wstring string_to_convert = path;

        //setup converter
        using convert_type = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_type, wchar_t> converter;

        //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
        std::string converted_str = converter.to_bytes( string_to_convert );
        
        return converted_str.substr(0,converted_str.size()-9);
        #endif
    }
}