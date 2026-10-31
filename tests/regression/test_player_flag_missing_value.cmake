# Bug: `--player` given as the last argument reads past the end of the
# argument list (every other flag checks for a missing value first).

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

apgn_run(result output ARGS "${WORK_DIR}/game.pgn" --player)
assert_contains("${output}" "does not have a value" "a missing --player value must be reported as such")
assert_equal("${result}" "1" "exit status for a missing --player value")
