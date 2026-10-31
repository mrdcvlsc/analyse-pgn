# Bug: the score of a search is read from whichever line the engine printed
# just before "bestmove". Engines may print other info lines there (e.g.
# "info string ..."), and then the score silently becomes 0.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

# every move is the engine's best move, scored +0.37 for the side to move
write_file("${WORK_DIR}/plan.txt" [=[
0 e2e4 cp:37 cp:37
1 e7e5 cp:37 cp:37
2 f1c4 cp:37 cp:37
3 b8c6 cp:37 cp:37
4 d1h5 cp:37 cp:37
5 g8f6 cp:37 cp:37
6 h5f7 cp:37 cp:37
]=])

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan.txt FAKE_ENGINE_MODE=info_string_after_pv
    ARGS --engine "${FAKE_ENGINE}" game.pgn)
assert_equal("${result}" "0" "apgn should succeed")

read_file(analysed "${WORK_DIR}/analyzed-game.pgn")
assert_contains("${analysed}" "CP:37" "the engine's score must be read from its scored info line")
assert_not_matches("${analysed}" "CP:0[^0-9]" "no move was scored 0 by the engine")
