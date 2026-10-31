# Bug: with --analyse-start-on-move, the moves before the start move are
# written without separating spaces ("e2e4e7e5..."), pgn-extract rejects the
# game, and the analysed PGN ends up with no moves at all.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

apgn_run(result output ARGS ${APGN_FAST_ARGS} --analyse-start-on-move 2 "${WORK_DIR}/game.pgn")
assert_equal("${result}" "0" "apgn should succeed")

read_file(analysed "${WORK_DIR}/analyzed-game.pgn")
assert_contains("${analysed}" "1. e4 e5 2. Bc4" "skipped opening moves must still be in the analysed game")
assert_contains("${analysed}" "Qxf7#" "the analysed game must contain every move")
assert_not_contains("${output}" "Unknown move text" "pgn-extract must be able to read the analysed game")
