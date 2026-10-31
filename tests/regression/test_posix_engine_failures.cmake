# Bug: when the chess engine stops (crashes, exits, or is not a UCI engine),
# apgn does not report it:
#  - end-of-file from the engine is treated as "no error", so apgn keeps
#    talking to a dead process and is killed by SIGPIPE without any message;
#  - check_for_error() ignores the message it is given, so every failure
#    reads "running chess engine failed" whatever actually went wrong.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

# 1. an engine that exits immediately
apgn_run(result output
    ENV FAKE_ENGINE_MODE=exit_on_start
    ARGS --engine "${FAKE_ENGINE}" --depth 2 game.pgn)
if(result EQUAL 0)
    fail("apgn must fail when the engine exits")
endif()
assert_contains("${output}" "End of file" "the engine closing its output must be reported")

# 2. an executable that is not a UCI engine at all
apgn_run(result output ARGS --engine /bin/echo --depth 2 game.pgn)
if(result EQUAL 0)
    fail("apgn must fail when the engine is not a UCI engine")
endif()
assert_matches("${output}" "failed to send 'uci' to engine|while waiting for uciok"
    "the error must say which step of talking to the engine failed")
