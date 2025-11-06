#include "score_comments.hpp"

std::string score_comments(int cp_pmvbmv_dif, int cp_pmv, int cp_bmv, int cm_pmv, int cm_bmv) {

    std::string comment = "";

    if (cm_pmv != 0) { // about to have checkmate
        if (cm_bmv >= 0 && cm_pmv < 0) {
            // player move is bad and is now allowed to be checkmated
            comment = " blunder ???";
        } else if (cm_pmv < 0) { // losing
            if (cm_bmv == cm_pmv) {
                comment = " accurate !!";
            } else if (std::abs(cm_bmv) > std::abs(cm_pmv)) {
                comment = " mistake ???";
            } else {
                comment = " wtf ?!!";
            }
        } else if (cm_pmv > 0) { // winning
            if (cm_pmv == cm_bmv) {
                comment = " excellent !!";
            } else if (cm_pmv > cm_bmv) {
                comment = " missed ???";
            } else {
                comment = " god-move !!!!";
            }
        }
    } else {
        if (cm_bmv > 0 && cm_pmv == 0 && cp_pmv >= 0) {
            // player is about to checkmate the opponent but failed to, yet player is still winning
            comment = " missed ??";
        } else if (cm_bmv > 0 && (cp_pmv < 0 || cm_pmv < 0)) {
            // player is about to checkmate the opponent but failed to, then player is about to get checkmated
            comment = " blunder ???";
        } else if (cp_pmvbmv_dif == 0) {
            if (cp_pmv > 0) {
                comment = " brilliant !!!"; // found the best move and is winning
            } else {
                comment = " accurate !!!"; // found the best move but is losing
            }
        } else if (cp_pmvbmv_dif > -10) {
            if (cp_pmv > 0) {
                comment = " excellent !!"; // found a top move and is winning
            } else {
                comment = " accurate !!"; // found a top move but is losing
            }
        } else if (cp_pmvbmv_dif > -35) {
            if (cp_pmv > 0) {
                comment = " good !"; // found a good move and is winning
            } else {
                comment = " questionable ?"; // found a good move but is losing
            }
        } else if (cp_pmvbmv_dif > -70) {
            comment = " inaccurate ?"; // found a move that is not bad but is also not good
        } else if (cp_pmvbmv_dif > -140) {
            comment = " mistake ??"; // found a bad move
        } else if (cp_pmvbmv_dif > -222) {
            comment = " mistake ???"; // found a really bad move
        } else {
            if (cp_pmv < 0) {
                comment = " blunder ???"; // found a losing move
            } else {
                comment = " missed ???"; // unable to find the winning move
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