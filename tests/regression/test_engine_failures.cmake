# Bug: when the chess engine stops (crashes, exits, or is not a UCI engine),
# apgn does not report it:
#  - end-of-file from the engine is treated as "no error", so apgn keeps
#    talking to a dead process and is killed by SIGPIPE without any message;
#  - check_for_error() ignores the message it is given, so every failure
#    reads "running chess engine failed" whatever actually went wrong.
#
# The system's error text differs per platform ("End of file" on POSIX,
# "The pipe has been ended" on Windows), so the test checks the exit status
# and apgn's own description of the step that failed.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

# 1. an engine that exits immediately
apgn_run(result output
    ENV FAKE_ENGINE_MODE=exit_on_start
    ARGS --engine "${FAKE_ENGINE}" --depth 2 game.pgn)
assert_equal("${result}" "1" "exit status when the engine exits immediately")
# apgn's first step is sending "uci" (the engine owes no banner before it), and
# whether that write or the wait for "uciok" fails first depends on timing
assert_matches("${output}" "failed to send 'uci' to engine|while waiting for uciok"
    "the engine stopping must be reported")

# 2. an executable that is not a UCI engine at all: cmake without arguments
#    prints its usage and exits
apgn_run(result output ARGS --engine "${CMAKE_COMMAND}" --depth 2 game.pgn)
assert_equal("${result}" "1" "exit status when the engine is not a UCI engine")
assert_matches("${output}" "failed to send 'uci' to engine|while waiting for uciok"
    "the error must say which step of talking to the engine failed")
