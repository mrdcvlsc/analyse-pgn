# Bug: errors are thrown as exceptions that nothing catches, so apgn aborts
# with "terminate called after throwing an instance of ..." (and a crash exit
# status) instead of printing an error and exiting with status 1.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

# no pgn file given
apgn_run(result output ARGS --depth 5)
assert_not_contains("${output}" "terminate called" "missing pgn argument must not crash apgn")
assert_contains("${output}" "no pgn files" "missing pgn argument must be reported")
assert_equal("${result}" "1" "exit status when no pgn file is given")

# pgn file that does not exist
apgn_run(result output ARGS ${APGN_FAST_ARGS} "${WORK_DIR}/does-not-exist.pgn")
assert_not_contains("${output}" "terminate called" "a missing pgn file must not crash apgn")
assert_equal("${result}" "1" "exit status when the pgn file does not exist")
