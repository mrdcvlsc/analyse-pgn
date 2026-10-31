# Bug: games that start from a custom position (SetUp/FEN tags) are analysed
# as if they started from the normal starting position, so the engine gets
# illegal moves and the analysed PGN is garbage. Games where black moves
# first are also attributed to the wrong colour.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/fen.pgn" [=[[Event "White to move"]
[Site "?"]
[Date "????.??.??"]
[Round "?"]
[White "A"]
[Black "B"]
[Result "*"]
[SetUp "1"]
[FEN "4k3/8/8/8/8/8/4P3/4K3 w - - 0 1"]

1. e4 Kd7 2. Kd2 Ke6 3. Ke3 *

[Event "Black to move"]
[Site "?"]
[Date "????.??.??"]
[Round "?"]
[White "A"]
[Black "B"]
[Result "*"]
[SetUp "1"]
[FEN "4k3/8/8/8/8/8/4P3/4K3 b - - 0 1"]

1... Kd7 2. e4 Ke6 3. Kd2 *
]=])

apgn_run(result output ARGS ${APGN_FAST_ARGS} --player Black "${WORK_DIR}/fen.pgn")
assert_equal("${result}" "0" "apgn should succeed")
assert_not_contains("${output}" "Failed to make move" "pgn-extract must accept every move apgn writes")
assert_not_contains("${output}" "Unknown" "pgn-extract must accept every move apgn writes")

read_file(analysed "${WORK_DIR}/analyzed-fen.pgn")
assert_contains("${analysed}" "White to move" "the first game must be in the analysed PGN")
assert_contains("${analysed}" "Black to move" "the second game must be in the analysed PGN")
assert_matches("${analysed}" "Kd2[ \n]+Ke6[ \n]*{" "first game: black's 2... Ke6 must be analysed")
assert_matches("${analysed}" "Kd7[ \n]*{" "second game: black's first move 1... Kd7 must be analysed")

read_file(stats "${WORK_DIR}/analyzed-stats-fen.json")
# second game: black moved twice (Kd7, Ke6), white's moves were not analysed (--player Black)
string(JSON black_color GET "${stats}" games 1 player_stats 1 player_color)
string(JSON black_moves LENGTH "${stats}" games 1 player_stats 1 centipawns)
string(JSON white_moves LENGTH "${stats}" games 1 player_stats 0 centipawns)
assert_equal("${black_color}" "black" "player_stats[1] is black")
assert_equal("${black_moves}" "2" "second game: analysed black moves")
assert_equal("${white_moves}" "0" "second game: analysed white moves with --player Black")
