# Bug: save_games() pipes the analysed games into pgn-extract, sleeps 1.5 s
# and then kills it. It never closes pgn-extract's stdin, never waits for it
# to finish and never checks its exit code, so a slow or failing conversion
# (large files, an unwritable output path) silently loses the analysis while
# apgn reports "pgn game saved".

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

# A directory where the output file should go makes pgn-extract fail
# ("Unable to open the file") with exit code 1.
file(MAKE_DIRECTORY "${WORK_DIR}/analyzed-game.pgn")

apgn_run(result output ARGS ${APGN_FAST_ARGS} --analyse-until-move 1 "${WORK_DIR}/game.pgn")
assert_not_contains("${output}" "pgn game saved" "apgn must not claim the game was saved when pgn-extract failed")
if(result EQUAL 0)
    fail("apgn must exit with an error when the analysed PGN could not be written")
endif()
