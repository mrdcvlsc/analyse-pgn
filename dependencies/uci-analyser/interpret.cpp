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
    short mistake[2];
    short blunder[2];
    short missedWin[2];
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
        mistake[0] = mistake[1] = 0;
        blunder[0] = blunder[1] = 0;
        missedWin[0] = missedWin[1] = 0;
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
        statCounts.append( "\tMistakes         - " + to_string(mistake[color]) + "\n");
        statCounts.append( "\tBlunders         - " + to_string(blunder[color]) + "\n");
        statCounts.append( "\tMissed Wins      - " + to_string(missedWin[color]) + "\n");
        statCounts.append( "\tTOTAL : " + to_string(
          ( brilliantMoves[color] + excellentMove[color] + goodMove[color] + inaccurateMove[color] +
            mistake[color] + blunder[color] + missedWin[color]))+"\n"
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

        if      (played_move>650) cout << " winning : ";
        else if(played_move<-650) cout << " losing : ";
    
        if(played_move==best_move)
        {
            brilliantMoves[color]++;
            cout<<" brilliant ";
        }
        else if(interpretation >  -10)
        {
            excellentMove[color]++;
            cout<<" excellent ";
        }
        else if(interpretation >  -60)
        {
            goodMove[color]++;
            cout<<" good ";
        }
        else if(interpretation > -110)
        {
            inaccurateMove[color]++;
            cout<<" inaccurate ";
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