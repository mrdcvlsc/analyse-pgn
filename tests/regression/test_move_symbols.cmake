# Feature: --move-symbols std|apgn|both writes the move assessment as standard
# NAGs instead of (or beside) apgn's own words, so other tools can read it
# (issue #16, third request).

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

foreach(name first second plain nags both)
    write_file("${WORK_DIR}/${name}.pgn" "${SCHOLARS_MATE_PGN}")
endforeach()

# <ply> <engine best move> <best move score> <played move score>
# every best move must be legal in its position, or pgn-extract rejects the
# variation apgn writes for it. the plies alternate white, black, white, ...
write_file("${WORK_DIR}/plan-first.txt" [=[
# 1. e4    best move, winning        -> brilliant !!! WINNING        -> $1 $18
0 d2d4 cp:300 cp:300
# 1... e5  best move, losing         -> accurate !!!  LOSING  (black) -> $1 $18
1 d7d5 cp:-200 cp:-200
# 2. Bc4   5 worse, level            -> excellent !!  UNCERTAIN      -> $10
2 g1f3 cp:55 cp:50
# 2... Nc6 50 worse, level           -> inaccurate ?  UNCERTAIN      -> $6 $10
3 g8f6 cp:50 cp:0
# 3. Qh5   100 worse, white ahead    -> mistake ??    ADVANTAGE      -> $2 $16
4 g1f3 cp:220 cp:120
# 3... Nf6 300 worse, losing (black) -> blunder !??   LOSING         -> $4 $18
5 g7g6 cp:-100 cp:-400
# 4. Qxf7# 300 worse, still winning  -> missed !??    WINNING        -> $2 $18
6 a2a4 cp:500 cp:200
]=])

write_file("${WORK_DIR}/plan-second.txt" [=[
# 1. e4    20 worse, level              -> good !         UNCERTAIN          -> $10
0 d2d4 cp:20 cp:0
# 1... e5  20 worse, losing             -> questionable ? LOSING     (black) -> $18
1 d7d5 cp:-180 cp:-200
# 2. Bc4   5 worse, losing              -> accurate !!    LOSING             -> $19
2 g1f3 cp:-195 cp:-200
# 2... Nc6 200 worse, losing            -> mistake ???    LOSING     (black) -> $2 $18
3 g8f6 cp:-50 cp:-250
# 3. Qh5   20 worse, white behind a bit -> questionable ? DISADVANTAGE       -> $17
4 g1f3 cp:-100 cp:-120
# 3... Nf6 5 worse, black behind a bit  -> accurate !!    DISADVANTAGE(black)-> $16
5 g7g6 cp:-120 cp:-125
# 4. Qxf7# 5 worse, white ahead a bit   -> excellent !!   ADVANTAGE          -> $16
6 a2a4 cp:130 cp:125
]=])

set(first_env FAKE_ENGINE_PLAN=${WORK_DIR}/plan-first.txt)
set(second_env FAKE_ENGINE_PLAN=${WORK_DIR}/plan-second.txt)

# pgn-extract wraps long lines, so a NAG can land on the line after its move:
# the assertions below allow a newline wherever they allow a space.

# ---- std mode replaces the words with NAGs ----

apgn_run(result output ENV ${first_env} ARGS --engine "${FAKE_ENGINE}" --move-symbols std first.pgn)
assert_equal("${result}" "0" "apgn should succeed with --move-symbols std")
assert_not_contains("${output}" "Unknown" "pgn-extract must accept the NAGs apgn writes")

read_file(analysed "${WORK_DIR}/analyzed-first.pgn")
assert_matches("${analysed}" "1\\.[ \n]+e4[ \n]+\\$1[ \n]+\\$18[ \n]+{" "1. e4: best move while winning")
assert_matches("${analysed}" "e5[ \n]+\\$1[ \n]+\\$18[ \n]+{" "1... e5: best move while losing, black is lost")
assert_matches("${analysed}" "Bc4[ \n]+\\$10[ \n]+{" "2. Bc4: a top move needs no move NAG")
assert_matches("${analysed}" "Nc6[ \n]+\\$6[ \n]+\\$10[ \n]+{" "2... Nc6: inaccuracy")
assert_matches("${analysed}" "Qh5[ \n]+\\$2[ \n]+\\$16[ \n]+{" "3. Qh5: mistake, white slightly ahead")
assert_matches("${analysed}" "Nf6[ \n]+\\$4[ \n]+\\$18[ \n]+{" "3... Nf6: blunder, black is lost")
assert_matches("${analysed}" "Qxf7#[ \n]+\\$2[ \n]+\\$18[ \n]+{" "4. Qxf7#: missed win")

# the words are gone in std mode, the score stays
assert_not_contains("${analysed}" "brilliant" "std mode must not write apgn's words")
assert_not_contains("${analysed}" "blunder" "std mode must not write apgn's words")
assert_not_contains("${analysed}" "WINNING" "std mode must not write apgn's status words")
assert_contains("${analysed}" "CP:300" "std mode still writes the score")

# in std mode the best-move variation holds only the score: the parentheses
# already mark it as the alternative, so apgn's wording is left out there too
assert_not_contains("${analysed}" "was the best move" "std mode must not write apgn's wording in variations")
assert_not_contains("${analysed}" "followed by" "std mode must not write apgn's wording in variations")
assert_matches("${analysed}" "\\(2\\.[ \n]+Nf3[ \n]+{[ \n]*CP:55[ \n]*}" "the best-move variation keeps its score in std mode")
assert_not_matches("${analysed}" "Nf3[ \n]+\\$" "the best-move variation gets no NAGs in std mode")

apgn_run(result output ENV ${second_env} ARGS --engine "${FAKE_ENGINE}" --move-symbols std second.pgn)
assert_equal("${result}" "0" "apgn should succeed with --move-symbols std")

read_file(analysed "${WORK_DIR}/analyzed-second.pgn")
assert_matches("${analysed}" "1\\.[ \n]+e4[ \n]+\\$10[ \n]+{" "1. e4: a good move needs no move NAG")
assert_matches("${analysed}" "e5[ \n]+\\$18[ \n]+{" "1... e5: black is lost")
assert_matches("${analysed}" "Bc4[ \n]+\\$19[ \n]+{" "2. Bc4: white is lost")
assert_matches("${analysed}" "Nc6[ \n]+\\$2[ \n]+\\$18[ \n]+{" "2... Nc6: mistake, black is lost")
assert_matches("${analysed}" "Qh5[ \n]+\\$17[ \n]+{" "3. Qh5: white slightly behind")
assert_matches("${analysed}" "Nf6[ \n]+\\$16[ \n]+{" "3... Nf6: black slightly behind is white slightly ahead")
assert_matches("${analysed}" "Qxf7#[ \n]+\\$16[ \n]+{" "4. Qxf7#: white slightly ahead")

# ---- apgn mode (and no flag at all) keep today's output ----

apgn_run(result output ENV ${first_env} ARGS --engine "${FAKE_ENGINE}" plain.pgn)
assert_equal("${result}" "0" "apgn should succeed without the flag")
apgn_run(result output ENV ${first_env} ARGS --engine "${FAKE_ENGINE}" --move-symbols apgn nags.pgn)
assert_equal("${result}" "0" "apgn should succeed with --move-symbols apgn")

read_file(plain "${WORK_DIR}/analyzed-plain.pgn")
read_file(explicit_apgn "${WORK_DIR}/analyzed-nags.pgn")
string(REPLACE "nags" "plain" explicit_apgn_renamed "${explicit_apgn}")
assert_equal("${explicit_apgn_renamed}" "${plain}" "--move-symbols apgn must match the output without the flag")
assert_not_contains("${plain}" "$1" "the default output has no NAGs")
assert_matches("${plain}" "1\\.[ \n]+e4[ \n]+{[ \n]*brilliant" "the default output keeps apgn's words")

# ---- both mode writes the NAG and the words ----

apgn_run(result output ENV ${first_env} ARGS --engine "${FAKE_ENGINE}" --move-symbols both both.pgn)
assert_equal("${result}" "0" "apgn should succeed with --move-symbols both")

read_file(analysed "${WORK_DIR}/analyzed-both.pgn")
assert_matches("${analysed}" "1\\.[ \n]+e4[ \n]+\\$1[ \n]+\\$18[ \n]+{[ \n]*brilliant[ \n]+!!![ \n]+WINNING" "both mode writes the NAG and the words")
assert_matches("${analysed}" "Nf6[ \n]+\\$4[ \n]+\\$18[ \n]+{[ \n]*blunder" "both mode writes the NAG and the words")

# ---- in both mode the best-move variation keeps apgn's wording, and gets no NAGs ----

assert_matches("${analysed}" "\\(2\\.[ \n]+Nf3[ \n]+{[^}]*was[ \n]+the[ \n]+best[ \n]+move" "the best-move variation keeps its wording in both mode")
assert_not_matches("${analysed}" "Nf3[ \n]+\\$" "the best-move variation gets no NAGs")

# ---- invalid mode ----

apgn_run(result output ENV ${first_env} ARGS --engine "${FAKE_ENGINE}" --move-symbols lichess first.pgn)
assert_equal("${result}" "1" "exit status for an unknown --move-symbols mode")
assert_contains("${output}" "invalid move-symbols value" "an unknown mode must be rejected")
