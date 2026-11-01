# Bug: the last move of a game is analysed by the engine, but its comment is
# never written to the analysed PGN (the output loop stops one move early).

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

apgn_run(result output ARGS ${APGN_FAST_ARGS} "${WORK_DIR}/game.pgn")
assert_equal("${result}" "0" "apgn should succeed")

read_file(analysed "${WORK_DIR}/analyzed-game.pgn")
# 7 plies in the game, every one of them must carry an analysis comment.
# e.g. "{ brilliant !!! WINNING CP:276 }"; pgn-extract may wrap lines anywhere.
count_matches(comments "${analysed}" "[A-Z]+[ \n]+(CP|MATE_IN):-?[0-9]+[ \n]+}")
assert_equal("${comments}" "7" "number of analysed moves with a comment")
assert_matches("${analysed}" "Qxf7#[ \n]+{" "the final move (4. Qxf7#) must have a comment")
