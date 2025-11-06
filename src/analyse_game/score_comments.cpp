#include "score_comments.hpp"

std::string score_comments(int cp_pmvbmv_dif, int cp_pmv, int cp_bmv, int cm_pmv, int cm_bmv) {

    std::string comment = "";

    if (cm_pmv != 0) { // about to have checkmate
        if (cm_bmv >= 0 && cm_pmv < 0) {
            // player move is bad and is now allowed to be checkmated
            comment = " " + comments::losing::BLUNDER;
        } else if (cm_pmv < 0) { // losing
            if (cm_bmv == cm_pmv) {
                comment = " " + comments::losing::ACCURATE_2;
            } else if (std::abs(cm_bmv) > std::abs(cm_pmv)) {
                comment = " " + comments::losing::MISTAKE_3;
            } else {
                comment = " wtf ?!!";
            }
        } else if (cm_pmv > 0) { // winning
            if (cm_pmv == cm_bmv) {
                comment = " " + comments::winning::EXCELLENT;
            } else if (cm_pmv > cm_bmv) {
                comment = " " + comments::winning::MISSED;
            } else {
                comment = " god-move !!!!";
            }
        }
    } else {
        if (cm_bmv > 0 && cm_pmv == 0 && cp_pmv >= 0) {
            // player is about to checkmate the opponent but failed to, yet player is still winning
            comment = " " + comments::winning::MISSED;
        } else if (cm_bmv > 0 && (cp_pmv < 0 || cm_pmv < 0)) {
            // player is about to checkmate the opponent but failed to, then player is about to get checkmated
            comment = " " + comments::losing::BLUNDER;
        } else if (cp_pmvbmv_dif == 0) {
            if (cp_pmv > 0) {
                comment = " " + comments::winning::BRILLIANT; // found the best move and is winning
            } else {
                comment = " " + comments::losing::ACCURATE_3; // found the best move but is losing
            }
        } else if (cp_pmvbmv_dif > -10) {
            if (cp_pmv > 0) {
                comment = " " + comments::winning::EXCELLENT; // found a top move and is winning
            } else {
                comment = " " + comments::losing::ACCURATE_2; // found a top move but is losing
            }
        } else if (cp_pmvbmv_dif > -35) {
            if (cp_pmv > 0) {
                comment = " " + comments::winning::GOOD; // found a good move and is winning
            } else {
                comment = " " + comments::losing::QUESTIONABLE; // found a good move but is losing
            }
        } else if (cp_pmvbmv_dif > -70) {
            comment = " " + comments::losing::INACCURATE; // found a move that is not bad but is also not good
        } else if (cp_pmvbmv_dif > -140) {
            comment = " " + comments::losing::MISTAKE_2; // found a bad move
        } else if (cp_pmvbmv_dif > -222) {
            comment = " " + comments::losing::MISTAKE_3; // found a really bad move
        } else {
            if (cp_pmv < 0) {
                comment = " " + comments::losing::BLUNDER; // found a losing move
            } else {
                comment = " " + comments::winning::MISSED; // unable to find the winning move
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