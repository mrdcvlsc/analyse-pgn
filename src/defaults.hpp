#pragma once

const auto APGN_VERSION = "version 2.0";
const auto SIZE_T_32BIT = 4;

const auto FLAG_VERSION = "--version";
const auto FLAG_HELP    = "--help";
const auto FLAG_ANALYSE = "-analyse";

const auto ANALYSE_ENGINE        = "-engine";
const auto ANALYSE_THREADS       = "-threads";
const auto ANALYSE_DEPTH         = "-depth";
const auto ANALYSE_COLOR         = "-color";
const auto ANALYSE_OPENNING_SKIP = "-oskip";
const auto ANALYSE_UNTIL         = "-movesuntil";

const auto DEFAULT_THREAD             = 1;
const auto DEFAULT_DEPTH              = 11;
const auto DEAFULT_OPENNING_MOVE_SKIP = 0;
const auto DEFAULT_MOVES_UNTIL        = 1'000;