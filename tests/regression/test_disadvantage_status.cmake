# Bug: the position status for a score between -115 and -150 is written as
# "DISDVANTAGE" (the README documents "DISADVANTAGE").

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

# every move is the engine's best move, scored -1.30 for the side to move
write_file("${WORK_DIR}/plan.txt" [=[
0 e2e4 cp:-130 cp:-130
1 e7e5 cp:-130 cp:-130
2 f1c4 cp:-130 cp:-130
3 b8c6 cp:-130 cp:-130
4 d1h5 cp:-130 cp:-130
5 g8f6 cp:-130 cp:-130
6 h5f7 cp:-130 cp:-130
]=])

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan.txt
    ARGS --engine "${FAKE_ENGINE}" game.pgn)
assert_equal("${result}" "0" "apgn should succeed")

read_file(analysed "${WORK_DIR}/analyzed-game.pgn")
assert_not_contains("${analysed}" "DISDVANTAGE" "status spelling")
assert_contains("${analysed}" "DISADVANTAGE CP:-130" "status for a score of -130")
