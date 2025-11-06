#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace comments {
namespace winning {
const std::pair<std::uint8_t, std::string> BRILLIANT = {1, "brilliant !!!"};
const std::pair<std::uint8_t, std::string> EXCELLENT = {2, "excellent !!"};
const std::pair<std::uint8_t, std::string> GOOD      = {3, "good !"};
const std::pair<std::uint8_t, std::string> MISSED    = {4, "missed !??"};
} // namespace winning

namespace losing {
const std::pair<std::uint8_t, std::string> ACCURATE_3   = {5, "accurate !!!"};
const std::pair<std::uint8_t, std::string> ACCURATE_2   = {6, "accurate !!"};
const std::pair<std::uint8_t, std::string> QUESTIONABLE = {7, "questionable ?"};
const std::pair<std::uint8_t, std::string> INACCURATE   = {8, "inaccurate ?"};
const std::pair<std::uint8_t, std::string> MISTAKE_2    = {9, "mistake ??"};
const std::pair<std::uint8_t, std::string> MISTAKE_3    = {10, "mistake ???"};
const std::pair<std::uint8_t, std::string> BLUNDER      = {11, "blunder !??"};
} // namespace losing

std::pair<std::uint8_t, std::string> rank(const std::string &comment);
} // namespace comments

std::string score_comments(int cp_pmvbmv_dif, int cp_pmv, int cp_bmv, int cm_pmv, int cm_bmv);