# Bug: the "pgn files to analyse" list numbers every file "1.)" because the
# counter is never incremented.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/first.pgn" "${SCHOLARS_MATE_PGN}")
write_file("${WORK_DIR}/second.pgn" "${SCHOLARS_MATE_PGN}")

apgn_run(result output ARGS ${APGN_FAST_ARGS} --analyse-until-move 1 first.pgn second.pgn)
assert_equal("${result}" "0" "apgn should succeed")
assert_contains("${output}" "1.) first.pgn" "first file in the list")
assert_contains("${output}" "2.) second.pgn" "second file in the list")
