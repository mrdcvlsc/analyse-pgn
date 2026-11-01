# Bug: engine moves are cut to 4 characters, so a promotion such as "b7a8q"
# becomes "b7a8". A player who plays the engine's best promotion is told the
# best move was something else, and the shown engine line starts with a
# stray "q" that pgn-extract cannot parse.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/promo.pgn" "${PROMOTION_PGN}")

apgn_run(result output ARGS ${APGN_FAST_ARGS} "${WORK_DIR}/promo.pgn")
assert_equal("${result}" "0" "apgn should succeed")
assert_not_contains("${output}" "Unknown move text" "pgn-extract must be able to read every move apgn writes")

read_file(analysed "${WORK_DIR}/analyzed-promo.pgn")
assert_contains("${analysed}" "bxa8=Q" "the promotion must be in the analysed game")
# 5. bxa8=Q is the engine's best move here; apgn must not suggest the very
# same move as a "better" alternative.
assert_not_matches("${analysed}" "\\(5\\.[ \n]+bxa8=Q" "the played promotion must not be listed as a missed best move")
