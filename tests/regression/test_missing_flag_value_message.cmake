# Bug: the error for a flag without a value is missing a space and the
# opening quote: "The given flag--depth' does not have a value".

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

apgn_run(result output ARGS "${WORK_DIR}/game.pgn" --depth)
assert_contains("${output}" "The given flag '--depth' does not have a value" "missing flag value message")
assert_equal("${result}" "1" "exit status for a missing flag value")
