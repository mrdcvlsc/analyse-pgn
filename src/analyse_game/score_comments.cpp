#include "score_comments.hpp"
#include <utility>

namespace comments {
std::pair<std::uint8_t, std::string> rank(const std::string &comment) {
    if (comment == winning::BRILLIANT.second) {
        return winning::BRILLIANT;
    } else if (comment == winning::EXCELLENT.second) {
        return winning::EXCELLENT;
    } else if (comment == winning::GOOD.second) {
        return winning::GOOD;
    } else if (comment == winning::MISSED.second) {
        return winning::MISSED;
    } else if (comment == losing::ACCURATE_3.second) {
        return losing::ACCURATE_3;
    } else if (comment == losing::ACCURATE_2.second) {
        return losing::ACCURATE_2;
    } else if (comment == losing::QUESTIONABLE.second) {
        return losing::QUESTIONABLE;
    } else if (comment == losing::INACCURATE.second) {
        return losing::INACCURATE;
    } else if (comment == losing::MISTAKE_2.second) {
        return losing::MISTAKE_2;
    } else if (comment == losing::MISTAKE_3.second) {
        return losing::MISTAKE_3;
    } else if (comment == losing::BLUNDER.second) {
        return losing::BLUNDER;
    }

    return std::make_pair(0, "{ apgn - rank function invalid argument error }");
}
} // namespace comments

std::string score_comments(int cp_pmvbmv_dif, int cp_pmv, int cp_bmv, int cm_pmv, int cm_bmv) {

    std::string comment = "";

    if (cm_pmv != 0) { // about to have checkmate
        if (cm_bmv >= 0 && cm_pmv < 0) {
            // player move is bad and is now allowed to be checkmated
            comment = " " + comments::losing::BLUNDER.second;
        } else if (cm_pmv < 0) { // losing
            if (cm_bmv == cm_pmv) {
                comment = " " + comments::losing::ACCURATE_2.second;
            } else if (std::abs(cm_bmv) > std::abs(cm_pmv)) {
                comment = " " + comments::losing::MISTAKE_3.second;
            } else {
                comment = " wtf ?!!";
            }
        } else if (cm_pmv > 0) { // winning
            if (cm_pmv == cm_bmv) {
                comment = " " + comments::winning::EXCELLENT.second;
            } else if (cm_pmv > cm_bmv) {
                comment = " " + comments::winning::MISSED.second;
            } else {
                comment = " god-move !!!!";
            }
        }
    } else {
        if (cm_bmv > 0 && cm_pmv == 0 && cp_pmv >= 0) {
            // player is about to checkmate the opponent but failed to, yet player is still winning
            comment = " " + comments::winning::MISSED.second;
        } else if (cm_bmv > 0 && (cp_pmv < 0 || cm_pmv < 0)) {
            // player is about to checkmate the opponent but failed to, then player is about to get checkmated
            comment = " " + comments::losing::BLUNDER.second;
        } else if (cp_pmvbmv_dif == 0) {
            if (cp_pmv > 0) {
                comment = " " + comments::winning::BRILLIANT.second; // found the best move and is winning
            } else {
                comment = " " + comments::losing::ACCURATE_3.second; // found the best move but is losing
            }
        } else if (cp_pmvbmv_dif > -10) {
            if (cp_pmv > 0) {
                comment = " " + comments::winning::EXCELLENT.second; // found a top move and is winning
            } else {
                comment = " " + comments::losing::ACCURATE_2.second; // found a top move but is losing
            }
        } else if (cp_pmvbmv_dif > -35) {
            if (cp_pmv > 0) {
                comment = " " + comments::winning::GOOD.second; // found a good move and is winning
            } else {
                comment = " " + comments::losing::QUESTIONABLE.second; // found a good move but is losing
            }
        } else if (cp_pmvbmv_dif > -70) {
            comment =
                " " + comments::losing::INACCURATE.second; // found a move that is not bad but is also not good
        } else if (cp_pmvbmv_dif > -140) {
            comment = " " + comments::losing::MISTAKE_2.second; // found a bad move
        } else if (cp_pmvbmv_dif > -222) {
            comment = " " + comments::losing::MISTAKE_3.second; // found a really bad move
        } else {
            if (cp_pmv < 0) {
                comment = " " + comments::losing::BLUNDER.second; // found a losing move
            } else {
                comment = " " + comments::winning::MISSED.second; // unable to find the winning move
            }
        }
    }

    if (cm_pmv > 0 || cp_pmv > 150) {
        comment += " WINNING";
    } else if (cm_pmv < 0 || cp_pmv < -150) {
        comment += " LOSING";
    } else if (cp_pmv > 115) {
        comment += " ADVANTAGE";
    } else if (cp_pmv < -115) {
        comment += " DISDVANTAGE";
    } else {
        comment += " UNCERTAIN";
    }

    return comment;
}