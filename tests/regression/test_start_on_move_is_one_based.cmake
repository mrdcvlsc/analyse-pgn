# Bug: --analyse-start-on-move N is documented as "the move number where the
# engine will start the analysis", but analysis actually starts at move N+1
# (plies are compared against N*2 instead of (N-1)*2).

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

# 1. e4 e5 2. Bc4 Nc6 3. Qh5 Nf6 4. Qxf7#  - starting on move 2 must analyse
# white's moves 2, 3, 4 and black's moves 2, 3.
apgn_run(result output ARGS ${APGN_FAST_ARGS} --analyse-start-on-move 2 "${WORK_DIR}/game.pgn")
assert_equal("${result}" "0" "apgn should succeed")

read_file(stats "${WORK_DIR}/analyzed-stats-game.json")
string(JSON white_moves LENGTH "${stats}" games 0 player_stats 0 centipawns)
string(JSON black_moves LENGTH "${stats}" games 0 player_stats 1 centipawns)
assert_equal("${white_moves}" "3" "white moves analysed when starting on move 2")
assert_equal("${black_moves}" "2" "black moves analysed when starting on move 2")
