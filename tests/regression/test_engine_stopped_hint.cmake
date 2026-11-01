# Bug: when the chess engine stops, apgn explains it with the hint
# "(the program stopped responding ...)" only for an end-of-file error.
# On Windows a closed pipe is reported as ERROR_BROKEN_PIPE ("The pipe has
# been ended") instead of end-of-file, and on every platform writing to an
# engine that has already exited fails with a broken pipe, so in those cases
# the error has no hint.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

apgn_run(result output
    ENV FAKE_ENGINE_MODE=exit_on_start
    ARGS --engine "${FAKE_ENGINE}" --depth 2 game.pgn)
assert_equal("${result}" "1" "exit status when the engine exits immediately")
assert_contains("${output}" "the program stopped responding" "a stopped engine must be explained on every platform")
