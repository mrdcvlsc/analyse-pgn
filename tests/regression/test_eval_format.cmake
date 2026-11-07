# Feature: --eval-format std|apgn|both writes the score of a move as a standard
# [%eval] tag instead of (or beside) apgn's CP:/MATE_IN: text, so lichess,
# chessbase and scid can read it (issue #16, fourth request).
#
# [%eval] values are from white's point of view, while apgn's CP:/MATE_IN:
# are from the moving player's, so the sign is flipped for black's moves.
#
# [%eval] is the evaluation of the position after the move, as lichess writes
# it, while apgn's MATE_IN counts the move itself. so when the moving player
# is the one giving mate, the tag counts one move fewer, and a move that
# delivers mate gets no tag at all (the game is over). when the moving player
# is the one getting mated, the count is the same.
#
# pgn-extract wraps long lines, so the assertions allow a newline wherever
# they allow a space.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

foreach(name std plain apgn both mates mates_standard)
    write_file("${WORK_DIR}/${name}.pgn" "${SCHOLARS_MATE_PGN}")
endforeach()

# <ply> <engine best move> <best move score> <played move score>
# every best move must be legal in its position, or pgn-extract rejects the
# variation apgn writes for it. the plies alternate white, black, white, ...
write_file("${WORK_DIR}/plan.txt" [=[
# 1. e4    white +25                      -> [%eval 0.25],  best move [%eval 0.30]
0 d2d4 cp:30 cp:25
# 1... e5  black -30, so white +30        -> [%eval 0.30]
1 d7d5 cp:-25 cp:-30
# 2. Bc4   level                          -> [%eval 0.00]
2 g1f3 cp:0 cp:0
# 2... Nc6 black +30, so white -30        -> [%eval -0.30]
3 g8f6 cp:35 cp:30
# 3. Qh5   white mates in 3 with this move -> 2 more after it: [%eval #2]
4 g1f3 cp:50 mate:3
# 3... Nf6 black mated in 2, so white #2  -> [%eval #2],     best move [%eval #1]
5 g7g6 mate:-1 mate:-2
# 4. Qxf7# white mated in 2               -> [%eval #-2]
6 a2a4 cp:20 mate:-2
]=])

set(engine_env FAKE_ENGINE_PLAN=${WORK_DIR}/plan.txt)

# ---- std mode replaces CP:/MATE_IN: with a standard tag ----

apgn_run(result output ENV ${engine_env} ARGS --engine "${FAKE_ENGINE}" --eval-format std std.pgn)
assert_equal("${result}" "0" "apgn should succeed with --eval-format std")
assert_not_contains("${output}" "Unknown" "pgn-extract must accept the eval tags apgn writes")

read_file(analysed "${WORK_DIR}/analyzed-std.pgn")
assert_matches("${analysed}" "1\\.[ \n]+e4[ \n]+{[ \n]*\\[%eval[ \n]+0\\.25\\]" "1. e4: white is 25 centipawns better")
assert_matches("${analysed}" "e5[ \n]+{[ \n]*\\[%eval[ \n]+0\\.30\\]" "1... e5: black's -30 is white's +0.30")
assert_matches("${analysed}" "Bc4[ \n]+{[ \n]*\\[%eval[ \n]+0\\.00\\]" "2. Bc4: a level position")
assert_matches("${analysed}" "Nc6[ \n]+{[ \n]*\\[%eval[ \n]+-0\\.30\\]" "2... Nc6: black's +30 is white's -0.30")
assert_matches("${analysed}" "Qh5[ \n]+{[ \n]*\\[%eval[ \n]+#2\\]" "3. Qh5: white mates in 3 counting this move, so in 2 after it")
assert_matches("${analysed}" "Nf6[ \n]+{[ \n]*\\[%eval[ \n]+#2\\]" "3... Nf6: black is mated in 2, so white mates in 2")
assert_matches("${analysed}" "Qxf7#[ \n]+{[ \n]*\\[%eval[ \n]+#-2\\]" "4. Qxf7#: white is mated in 2")

assert_not_contains("${analysed}" "CP:" "std mode must not write apgn's score text")
assert_not_contains("${analysed}" "MATE_IN:" "std mode must not write apgn's mate text")
assert_contains("${analysed}" "excellent !!" "--eval-format only changes the score, the words stay")

# the best-move variation carries a tag of its own
assert_matches("${analysed}" "\\(1\\.[ \n]+d4[ \n]+{[ \n]*\\[%eval[ \n]+0\\.30\\][ \n]+was[ \n]+the[ \n]+best[ \n]+move,"
    "the best move's score is a tag too")
assert_matches("${analysed}" "\\(3\\.\\.\\.[ \n]+g6[ \n]+{[ \n]*\\[%eval[ \n]+#1\\]" "a mate score in a variation")

# ---- apgn mode (and no flag at all) keep today's output ----

apgn_run(result output ENV ${engine_env} ARGS --engine "${FAKE_ENGINE}" plain.pgn)
assert_equal("${result}" "0" "apgn should succeed without the flag")
apgn_run(result output ENV ${engine_env} ARGS --engine "${FAKE_ENGINE}" --eval-format apgn apgn.pgn)
assert_equal("${result}" "0" "apgn should succeed with --eval-format apgn")

read_file(plain "${WORK_DIR}/analyzed-plain.pgn")
read_file(explicit_apgn "${WORK_DIR}/analyzed-apgn.pgn")
assert_equal("${explicit_apgn}" "${plain}" "--eval-format apgn must match the output without the flag")
assert_not_contains("${plain}" "%eval" "the default output has no eval tags")
assert_contains("${plain}" "CP:25" "the default output keeps apgn's score text")

# ---- both mode writes the tag and apgn's text ----

apgn_run(result output ENV ${engine_env} ARGS --engine "${FAKE_ENGINE}" --eval-format both both.pgn)
assert_equal("${result}" "0" "apgn should succeed with --eval-format both")

read_file(analysed "${WORK_DIR}/analyzed-both.pgn")
assert_matches("${analysed}" "1\\.[ \n]+e4[ \n]+{[ \n]*\\[%eval[ \n]+0\\.25\\][ \n]+excellent" "both mode writes the tag and the words")
assert_contains("${analysed}" "CP:25" "both mode keeps apgn's score text")
assert_matches("${analysed}" "Qh5[ \n]+{[ \n]*\\[%eval[ \n]+#2\\]" "both mode writes the tag for a mate too")
assert_contains("${analysed}" "MATE_IN:3" "both mode keeps apgn's mate text")

# ---- giving mate: the count after the move, and no tag once the game is over ----

write_file("${WORK_DIR}/plan-mates.txt" [=[
0 d2d4 cp:30 cp:25
1 d7d5 cp:-25 cp:-30
2 g1f3 cp:0 cp:0
# 2... Nc6  black mates in 4 with this move -> 3 more after it: [%eval #-3]
3 g8f6 cp:35 mate:4
# 3. Qh5    the best move 3. Nf3 would deliver mate, so its variation has no tag
4 g1f3 mate:1 cp:40
5 g7g6 cp:0 cp:-10
# 4. Qxf7#  delivers mate: the game is over after it, so no tag
6 a2a4 cp:20 mate:1
]=])
set(mates_env FAKE_ENGINE_PLAN=${WORK_DIR}/plan-mates.txt)

apgn_run(result output ENV ${mates_env} ARGS --engine "${FAKE_ENGINE}" --eval-format std mates.pgn)
assert_equal("${result}" "0" "apgn should succeed with --eval-format std")
assert_not_contains("${output}" "Unknown" "pgn-extract must accept the output")

read_file(analysed "${WORK_DIR}/analyzed-mates.pgn")
assert_matches("${analysed}" "Nc6[ \n]+{[ \n]*\\[%eval[ \n]+#-3\\]" "2... Nc6: black mates in 4 counting this move, so in 3 after it")
assert_matches("${analysed}" "\\(3\\.[ \n]+Nf3[ \n]+{[ \n]*was[ \n]+the[ \n]+best[ \n]+move" "a best move that delivers mate keeps its words")
assert_not_matches("${analysed}" "\\(3\\.[ \n]+Nf3[ \n]+{[ \n]*\\[%eval" "a best move that delivers mate gets no tag")
assert_matches("${analysed}" "Qxf7#[ \n]+{[ \n]*[a-z]" "4. Qxf7# keeps its words")
assert_not_matches("${analysed}" "Qxf7#[ \n]+{[ \n]*\\[%eval" "4. Qxf7# delivers mate: no tag")

# with --move-symbols std as well, a comment left with nothing in it is not written at all
apgn_run(result output ENV ${mates_env} ARGS --engine "${FAKE_ENGINE}" --eval-format std --move-symbols std mates_standard.pgn)
assert_equal("${result}" "0" "apgn should succeed with both std modes")
assert_not_contains("${output}" "Unknown" "pgn-extract must accept the output")

read_file(analysed "${WORK_DIR}/analyzed-mates_standard.pgn")
assert_matches("${analysed}" "Qxf7#[ \n]+\\$[0-9]+[ \n]+\\$[0-9]+[ \n]+\\(4\\.[ \n]+a4[ \n]+{[ \n]*\\[%eval[ \n]+0\\.20\\][ \n]*}"
    "4. Qxf7#: its NAGs, no comment, then its variation")
assert_matches("${analysed}" "\\(3\\.[ \n]+Nf3[ \n]*\\)" "the variation of a best move that delivers mate has no comment")
assert_not_matches("${analysed}" "{[ \n]*}" "no empty comments")

# ---- invalid mode ----

apgn_run(result output ENV ${engine_env} ARGS --engine "${FAKE_ENGINE}" --eval-format lichess std.pgn)
assert_equal("${result}" "1" "exit status for an unknown --eval-format mode")
assert_contains("${output}" "invalid eval-format value" "an unknown mode must be rejected")
