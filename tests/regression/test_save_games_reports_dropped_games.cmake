# Bug: when pgn-extract rejects an analysed game while writing the analysed
# PGN (an illegal move in it), it leaves that game out and still exits with
# status 0. apgn only checks the exit status, so it reports "pgn game saved"
# and exits with 0 although games are missing from the file.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

# the plan below fits the first game. in the second game, 2. f1c4 (the
# engine's best move at ply 2) is illegal: the pawn on e2 blocks the bishop,
# so the best-move variation apgn writes for 2. c4 makes pgn-extract reject
# that whole game.
write_file("${WORK_DIR}/games.pgn" "${SCHOLARS_MATE_PGN}
[Event \"Rejected\"]
[Site \"?\"]
[Date \"????.??.??\"]
[Round \"?\"]
[White \"C\"]
[Black \"D\"]
[Result \"*\"]

1. d4 d5 2. c4 e6 3. Nc3 Nf6 4. Bg5 *
")

write_file("${WORK_DIR}/plan.txt" [=[
0 g1f3 cp:30 cp:30
1 g8f6 cp:0 cp:0
2 f1c4 cp:30 cp:30
3 b8c6 cp:0 cp:0
4 b1c3 cp:30 cp:30
5 g8f6 cp:0 cp:0
6 a2a4 cp:30 cp:30
]=])

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan.txt
    ARGS --engine "${FAKE_ENGINE}" games.pgn)

assert_contains("${output}" "pgn-extract rejected 1 of the 2 analysed games"
    "apgn must say that an analysed game is missing from the file")
assert_not_contains("${output}" "pgn game saved" "apgn must not claim every game was saved")
if(result EQUAL 0)
    fail("apgn must exit with an error when analysed games are missing from the output")
endif()

# what pgn-extract accepted is kept, and the statistics are still written
read_file(analysed "${WORK_DIR}/analyzed-games.pgn")
assert_contains("${analysed}" "Qxf7#" "the accepted game must still be in the output")
assert_not_contains("${analysed}" "Rejected" "the rejected game is not in the output")
assert_exists("${WORK_DIR}/analyzed-stats-games.json" "the statistics must still be written")
