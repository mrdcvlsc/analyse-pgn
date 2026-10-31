# Bug: --analyse-until-move N is a move number, but it is compared against the
# ply index, so `--analyse-until-move 2` only analyses move 1.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

# 1. e4 e5 2. Bc4 Nc6 ... - analysing until move 2 covers moves 1 and 2 of both sides.
apgn_run(result output ARGS ${APGN_FAST_ARGS} --analyse-until-move 2 "${WORK_DIR}/game.pgn")
assert_equal("${result}" "0" "apgn should succeed")

read_file(stats "${WORK_DIR}/analyzed-stats-game.json")
string(JSON white_moves LENGTH "${stats}" games 0 player_stats 0 centipawns)
string(JSON black_moves LENGTH "${stats}" games 0 player_stats 1 centipawns)
assert_equal("${white_moves}" "2" "white moves analysed until move 2")
assert_equal("${black_moves}" "2" "black moves analysed until move 2")
