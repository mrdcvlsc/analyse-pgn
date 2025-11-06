#pragma once

#include <string>

namespace comments {
namespace winning {
const std::string BRILLIANT = "brilliant !!!";
const std::string EXCELLENT = "excellent !!";
const std::string GOOD      = "good !";
const std::string MISSED    = "missed !??";
} // namespace winning

namespace losing {
const std::string ACCURATE_3   = "accurate !!!";
const std::string ACCURATE_2   = "accurate !!";
const std::string QUESTIONABLE = "questionable ?";
const std::string INACCURATE   = "inaccurate ?";
const std::string MISTAKE_2    = "mistake ??";
const std::string MISTAKE_3    = "mistake ???";
const std::string BLUNDER      = "blunder !??";
} // namespace losing

} // namespace comments

std::string score_comments(int cp_pmvbmv_dif, int cp_pmv, int cp_bmv, int cm_pmv, int cm_bmv);