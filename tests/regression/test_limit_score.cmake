# Feature: --limitscore N (pawns) stops analysing a game once it is decided,
# so a game that is already won or lost is not analysed to the last move
# (issue #16, second request).

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")
write_file("${WORK_DIR}/mate.pgn" "${SCHOLARS_MATE_PGN}")
write_file("${WORK_DIR}/inside.pgn" "${SCHOLARS_MATE_PGN}")

string(REPLACE "Scholar's mate" "Second game" SECOND_GAME "${SCHOLARS_MATE_PGN}")
write_file("${WORK_DIR}/two.pgn" "${SCHOLARS_MATE_PGN}\n${SECOND_GAME}")

# <ply> <engine best move> <best move score> <played move score>
# every best move must be legal in its position, or pgn-extract rejects the
# variation apgn writes for it. plies after the one that crosses the limit are
# left out of the plan on purpose: if apgn asks for them, the fake engine
# complains about a missing plan and the test notices.
write_file("${WORK_DIR}/plan-limit.txt" [=[
0 d2d4 cp:50 cp:30
1 d7d5 cp:0 cp:-20
# 2. Bc4 reaches +2.14 for white, at or beyond a 2.00 limit
2 g1f3 cp:250 cp:214
]=])

write_file("${WORK_DIR}/plan-mate.txt" [=[
# 1. e4 forces mate, which decides the game whatever the limit is
0 d2d4 cp:50 mate:4
]=])

write_file("${WORK_DIR}/plan-inside.txt" [=[
0 d2d4 cp:50 cp:30
1 d7d5 cp:0 cp:-20
2 g1f3 cp:60 cp:40
3 g8f6 cp:0 cp:-30
4 g1f3 cp:50 cp:20
5 g7g6 cp:0 cp:-40
6 a2a4 cp:20 cp:10
]=])

set(comment_regex "[A-Z]+[ \n]+(CP|MATE_IN):-?[0-9]+[ \n]*}")

# ---- the score crosses the limit in the middle of the game ----

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan-limit.txt
    ARGS --engine "${FAKE_ENGINE}" --limitscore 2.00 game.pgn)
assert_equal("${result}" "0" "apgn should succeed")
assert_contains("${output}" "skipping the rest of this game" "apgn must say that it stopped early")
assert_contains("${output}" "+2.14" "the message must name the score that crossed the limit")
assert_contains("${output}" "on move 2" "the message must name the move it stopped on")
assert_not_contains("${output}" "no plan for ply" "the engine must not be asked about the moves after the limit")

read_file(analysed "${WORK_DIR}/analyzed-game.pgn")
count_matches(comments "${analysed}" "${comment_regex}")
assert_equal("${comments}" "3" "annotated moves before the limit was reached")
assert_matches("${analysed}" "Bc4[ \n]+{[^}]*CP:214" "the move that crossed the limit keeps its annotation")
assert_not_matches("${analysed}" "Nc6[ \n]+{" "2... Nc6 comes after the limit, it must not be annotated")
assert_not_matches("${analysed}" "Qxf7#[ \n]+{" "4. Qxf7# comes after the limit, it must not be annotated")
assert_contains("${analysed}" "Qxf7#" "the moves after the limit must still be in the output")

read_file(stats "${WORK_DIR}/analyzed-stats-game.json")
set(white_moves 0)
set(black_moves 0)
string(JSON player_count LENGTH "${stats}" games 0 player_stats)
math(EXPR last_player "${player_count} - 1")
foreach(index RANGE ${last_player})
    string(JSON color GET "${stats}" games 0 player_stats ${index} player_color)
    string(JSON count LENGTH "${stats}" games 0 player_stats ${index} centipawns)
    set(${color}_moves ${count})
endforeach()
assert_equal("${white_moves}" "2" "the statistics count only the analysed white moves")
assert_equal("${black_moves}" "1" "the statistics count only the analysed black moves")

# ---- a forced mate decides the game too ----

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan-mate.txt
    ARGS --engine "${FAKE_ENGINE}" --limitscore 2.00 mate.pgn)
assert_equal("${result}" "0" "apgn should succeed")
assert_contains("${output}" "forced mate" "a forced mate must be reported as the reason for stopping")
assert_contains("${output}" "on move 1" "the message must name the move it stopped on")
assert_not_contains("${output}" "no plan for ply" "the engine must not be asked about the moves after the mate")

read_file(analysed "${WORK_DIR}/analyzed-mate.pgn")
count_matches(comments "${analysed}" "${comment_regex}")
assert_equal("${comments}" "1" "only the move that found the mate is annotated")

# ---- a game that stays inside the limit is analysed to the end ----

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan-inside.txt
    ARGS --engine "${FAKE_ENGINE}" --limitscore 2.00 inside.pgn)
assert_equal("${result}" "0" "apgn should succeed")
assert_not_contains("${output}" "skipping the rest" "a game inside the limit must be analysed to the end")

read_file(analysed "${WORK_DIR}/analyzed-inside.pgn")
count_matches(comments "${analysed}" "${comment_regex}")
assert_equal("${comments}" "7" "every move of a game inside the limit is annotated")

# ---- the limit applies per game: the next game is still analysed ----

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan-limit.txt
    ARGS --engine "${FAKE_ENGINE}" --limitscore 2.00 two.pgn)
assert_equal("${result}" "0" "apgn should succeed")

read_file(analysed "${WORK_DIR}/analyzed-two.pgn")
assert_contains("${analysed}" "Second game" "both games must be in the output")
count_matches(comments "${analysed}" "${comment_regex}")
assert_equal("${comments}" "6" "both games are analysed until they reach the limit")

# ---- invalid values ----

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan-limit.txt
    ARGS --engine "${FAKE_ENGINE}" --limitscore 0 game.pgn)
assert_equal("${result}" "1" "exit status for --limitscore 0")
assert_contains("${output}" "must be between" "an out-of-range limit must be rejected with its range")

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan-limit.txt
    ARGS --engine "${FAKE_ENGINE}" --limitscore 1.234 game.pgn)
assert_equal("${result}" "1" "exit status for --limitscore 1.234")
assert_contains("${output}" "invalid limitscore value" "a malformed limit must be rejected")
