# Feature: --min-loss N (pawns) leaves moves that are less than N worse than
# the engine's best move unannotated, so small, uninteresting losses do not
# clutter the output (issue #16, first request).

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")
file(COPY "${WORK_DIR}/game.pgn" DESTINATION "${WORK_DIR}/copy")
file(RENAME "${WORK_DIR}/copy/game.pgn" "${WORK_DIR}/same.pgn")

# <ply> <engine best move> <best move score> <played move score>
# the played move is never the engine's best move, so every ply is given an
# explicit centipawn loss (best score - played score). every best move must be
# legal in its position, or pgn-extract rejects the variation apgn writes for it.
write_file("${WORK_DIR}/plan.txt" [=[
# 1. e4      loses 20 -> under a 0.50 minimum loss
0 d2d4 cp:50 cp:30
# 1... e5    loses exactly 50 -> at the minimum loss, still annotated
1 d7d5 cp:0 cp:-50
# 2. Bc4     loses 200 -> annotated
2 g1f3 cp:100 cp:-100
# 2... Nc6   loses 5 -> under the minimum loss
3 g8f6 cp:0 cp:-5
# 3. Qh5     forces mate -> always annotated
4 g1f3 cp:50 mate:3
# 3... Nf6   gets mated -> always annotated
5 g7g6 mate:-2 mate:-5
# 4. Qxf7#   loses 10 -> under the minimum loss
6 a2a4 cp:20 cp:10
]=])

set(engine_env FAKE_ENGINE_PLAN=${WORK_DIR}/plan.txt)
set(comment_regex "[A-Z]+[ \n]+(CP|MATE_IN):-?[0-9]+[ \n]*}")

# ---- without the flag every analysed move is annotated, as before ----

apgn_run(result output ENV ${engine_env} ARGS --engine "${FAKE_ENGINE}" game.pgn)
assert_equal("${result}" "0" "apgn should succeed without the flag")

read_file(analysed "${WORK_DIR}/analyzed-game.pgn")
count_matches(comments "${analysed}" "${comment_regex}")
assert_equal("${comments}" "7" "annotated moves without --min-loss")

# ---- with --min-loss 0.50 only the losses of 50 centipawns or more remain ----

apgn_run(result output ENV ${engine_env} ARGS --engine "${FAKE_ENGINE}" --min-loss 0.50 same.pgn)
assert_equal("${result}" "0" "apgn should succeed with --min-loss 0.50")

read_file(filtered "${WORK_DIR}/analyzed-same.pgn")
count_matches(comments "${filtered}" "${comment_regex}")
assert_equal("${comments}" "4" "annotated moves with --min-loss 0.50")

assert_not_matches("${filtered}" "1\\. e4[ \n]+{" "1. e4 loses 20, it must not be annotated")
assert_not_matches("${filtered}" "Nc6[ \n]+{" "2... Nc6 loses 5, it must not be annotated")
assert_not_matches("${filtered}" "Qxf7#[ \n]+{" "4. Qxf7# loses 10, it must not be annotated")
assert_matches("${filtered}" "e5[ \n]+{[ \n]*inaccurate" "1... e5 loses exactly 50, it must be annotated")
assert_matches("${filtered}" "Bc4[ \n]+{" "2. Bc4 loses 200, it must be annotated")
assert_matches("${filtered}" "Qh5[ \n]+{[^}]*MATE_IN:3" "3. Qh5 forces mate, it must be annotated")
assert_matches("${filtered}" "Nf6[ \n]+{[^}]*MATE_IN:-5" "3... Nf6 gets mated, it must be annotated")

# a move that is not annotated keeps no best-move variation either
assert_not_matches("${filtered}" "\\(1\\. d4" "1. e4 is under the minimum loss: no best-move variation")
assert_matches("${filtered}" "\\(2\\. Nf3" "2. Bc4 is above the minimum loss: it keeps its best-move variation")

# ---- the statistics are unaffected by the flag ----

read_file(stats_all "${WORK_DIR}/analyzed-stats-game.json")
read_file(stats_filtered "${WORK_DIR}/analyzed-stats-same.json")
assert_equal("${stats_filtered}" "${stats_all}" "--min-loss must not change the statistics")

# ---- invalid values ----

apgn_run(result output ENV ${engine_env} ARGS --engine "${FAKE_ENGINE}" --min-loss 10.01 game.pgn)
assert_equal("${result}" "1" "exit status for --min-loss 10.01")
assert_contains("${output}" "must be between" "an out-of-range minimum loss must be rejected with its range")

apgn_run(result output ENV ${engine_env} ARGS --engine "${FAKE_ENGINE}" --min-loss abc game.pgn)
assert_equal("${result}" "1" "exit status for --min-loss abc")
assert_contains("${output}" "invalid min-loss value" "a malformed minimum loss must be rejected")

# ---- leading zeros do not change a value ----
# "0000000000.50" is 0.50; a whole part longer than 9 characters used to be
# rejected as out of range even when it was only zeros.

write_file("${WORK_DIR}/zeros.pgn" "${SCHOLARS_MATE_PGN}")
apgn_run(result output ENV ${engine_env} ARGS --engine "${FAKE_ENGINE}" --min-loss 0000000000.50 zeros.pgn)
assert_equal("${result}" "0" "--min-loss 0000000000.50 is 0.50 and must be accepted")

read_file(zeros "${WORK_DIR}/analyzed-zeros.pgn")
count_matches(comments "${zeros}" "${comment_regex}")
assert_equal("${comments}" "4" "--min-loss 0000000000.50 behaves like --min-loss 0.50")
