# Bug: an argument is treated as a pgn file if it merely contains the text
# "pgn" anywhere (case-sensitive), so "GAME.PGN" is ignored while a file such
# as "pgn-notes.txt" is passed to pgn-extract as a game file.
#
# Arguments are relative to WORK_DIR: the absolute path itself contains
# "analyse-pgn", which would hide the bug.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

# an upper-case .PGN extension is still a pgn file
write_file("${WORK_DIR}/GAME.PGN" "${SCHOLARS_MATE_PGN}")
apgn_run(result output ARGS ${APGN_FAST_ARGS} --analyse-until-move 1 GAME.PGN)
assert_equal("${result}" "0" "apgn should analyse GAME.PGN")
assert_exists("${WORK_DIR}/analyzed-GAME.PGN" "GAME.PGN must be analysed")

# an argument that only contains "pgn" in its name is not a pgn file
write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")
apgn_run(result output ARGS ${APGN_FAST_ARGS} --analyse-until-move 1 game.pgn pgn-notes.txt)
assert_equal("${result}" "0" "a non-.pgn argument must not be treated as a game file")
assert_contains("${output}" "ignoring the unknown parameter 'pgn-notes.txt" "non-.pgn argument is ignored")
assert_exists("${WORK_DIR}/analyzed-game.pgn" "game.pgn must still be analysed")
