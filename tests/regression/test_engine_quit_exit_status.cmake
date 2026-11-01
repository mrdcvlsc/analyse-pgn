# Bug: after the analysis apgn sends "quit" to the engine and then
# immediately calls request_exit(), which terminates the engine (SIGTERM on
# POSIX) before it can exit by itself. The engine then reports a non-zero
# exit status (15) and every analysed game ends with the false warning
# "warning: an error might have occured".

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

apgn_run(result output ARGS ${APGN_FAST_ARGS} --analyse-until-move 1 game.pgn)
assert_equal("${result}" "0" "apgn should succeed")
assert_not_contains("${output}" "an error might have occured" "a normal engine shutdown must not be reported as an error")
assert_contains("${output}" "uci engine return status : 0" "the engine must exit by itself with status 0 after quit")
