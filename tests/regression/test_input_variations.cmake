# Bug: games are loaded with pgn-extract without --novars, so the variations
# of an annotated game (apgn's own output included) stay in the move list:
# "(d2d4" and "d7d5)" are read as moves of the game. The engine then analyses
# the wrong positions, pgn-extract rejects the analysed game, and the
# analysed PGN file is left empty.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

# Scholar's mate with a variation, a comment and a NAG, as annotated games have
write_file("${WORK_DIR}/game.pgn" [=[[Event "With a variation"]
[Site "?"]
[Date "????.??.??"]
[Round "?"]
[White "A"]
[Black "B"]
[Result "1-0"]

1. e4 $1 { the main line } (1. d4 d5 2. c4) 1... e5 2. Bc4 Nc6 3. Qh5 Nf6 4. Qxf7# 1-0
]=])

# the engine agrees with every move of the main line, so apgn writes no
# variation of its own: any parenthesis in the output comes from the input.
# the fake engine counts the moves it is given, so a variation move read as a
# move of the game shifts the plies and makes it complain about a missing plan.
write_file("${WORK_DIR}/plan.txt" [=[
0 e2e4 cp:30 cp:30
1 e7e5 cp:-30 cp:-30
2 f1c4 cp:30 cp:30
3 b8c6 cp:-30 cp:-30
4 d1h5 cp:40 cp:40
5 g8f6 cp:-300 cp:-300
6 h5f7 mate:1 mate:1
]=])

set(comment_regex "[A-Z]+[ \n]+(CP|MATE_IN):-?[0-9]+[ \n]*}")

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan.txt
    ARGS --engine "${FAKE_ENGINE}" game.pgn)
assert_equal("${result}" "0" "apgn should succeed")
assert_not_contains("${output}" "no plan for ply" "the variation's moves must not be sent to the engine")
assert_not_contains("${output}" "Failed to make move" "pgn-extract must accept the analysed game")

read_file(analysed "${WORK_DIR}/analyzed-game.pgn")
assert_contains("${analysed}" "Qxf7#" "the analysed game must be in the output")
count_matches(comments "${analysed}" "${comment_regex}")
assert_equal("${comments}" "7" "every move of the main line is annotated")
assert_not_contains("${analysed}" "(" "the input's variation is not part of the analysed game")
assert_not_contains("${analysed}" "the main line" "the input's comments are not part of the analysed game")
