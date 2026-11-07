# Bug: apgn waits for a first line from the engine before it sends "uci".
# Stockfish prints a banner when it starts, but UCI does not require one, and
# an engine that waits silently for its first command makes apgn hang forever.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

write_file("${WORK_DIR}/plan.txt" [=[
0 d2d4 cp:30 cp:30
1 d7d5 cp:0 cp:0
2 g1f3 cp:30 cp:30
3 g8f6 cp:0 cp:0
4 g1f3 cp:30 cp:30
5 g7g6 cp:0 cp:0
6 a2a4 cp:30 cp:30
]=])

apgn_run(result output
    ENV FAKE_ENGINE_MODE=no_banner FAKE_ENGINE_PLAN=${WORK_DIR}/plan.txt
    TIMEOUT 60
    ARGS --engine "${FAKE_ENGINE}" game.pgn)
assert_equal("${result}" "0" "apgn must not wait for a banner from the engine")

read_file(analysed "${WORK_DIR}/analyzed-game.pgn")
assert_contains("${analysed}" "Qxf7#" "the game must be analysed and saved")
