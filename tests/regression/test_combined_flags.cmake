# The four annotation flags of issue #16 work together: --min-loss decides,
# once, whether a move's whole annotation is written (NAGs and eval tags
# included), --stop-at-score ends the analysis wherever it triggers, and
# --move-symbols std with --eval-format std leaves no apgn wording at all.
#
# pgn-extract wraps long lines, so the assertions allow a newline wherever
# they allow a space.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

foreach(name loss_symbols stop_eval loss_stop standard all)
    write_file("${WORK_DIR}/${name}.pgn" "${SCHOLARS_MATE_PGN}")
endforeach()

# <ply> <engine best move> <best move score> <played move score>
# every best move must be legal in its position, or pgn-extract rejects the
# variation apgn writes for it. plies after a stop are left out of a plan on
# purpose: if apgn asks for them, the fake engine complains and the test
# notices.
write_file("${WORK_DIR}/plan-mixed.txt" [=[
# 1. e4    loses 0 (best move)  -> hidden under a 0.50 minimum loss
0 d2d4 cp:30 cp:30
# 1... e5  loses 50             -> inaccurate ? UNCERTAIN (black)  -> $6 $10
1 d7d5 cp:50 cp:0
# 2. Bc4   loses 5              -> hidden
2 g1f3 cp:55 cp:50
# 2... Nc6 loses 300            -> blunder !?? LOSING (black)       -> $4 $18
3 g8f6 cp:-100 cp:-400
# the rest lose 10 or less      -> hidden
4 g1f3 cp:50 cp:40
5 g7g6 cp:0 cp:-10
6 a2a4 cp:20 cp:10
]=])

write_file("${WORK_DIR}/plan-stop.txt" [=[
0 d2d4 cp:30 cp:25
1 d7d5 cp:-25 cp:-30
# 2. Bc4 reaches +2.14 for white: the analysis stops here
2 g1f3 cp:250 cp:214
]=])

write_file("${WORK_DIR}/plan-loss-stop.txt" [=[
# 1. e4    loses 20             -> hidden
0 d2d4 cp:50 cp:30
# 1... e5  loses 60             -> annotated
1 d7d5 cp:0 cp:-60
# 2. Bc4   the best move (loses 0) at +2.14: hidden, and the analysis stops
2 g1f3 cp:214 cp:214
]=])

write_file("${WORK_DIR}/plan-all.txt" [=[
# 1. e4    loses 20             -> hidden
0 d2d4 cp:50 cp:30
# 1... e5  loses 60, level      -> $6 $10, black's -0.60 is white's +0.60
1 d7d5 cp:0 cp:-60
# 2. Bc4   loses 100 at +2.50   -> $2 $18, and the analysis stops
2 g1f3 cp:350 cp:250
]=])

# every NAG, and every comment that is nothing but an eval tag. counts never
# include a '[' in the matched text: cmake does not split list items inside an
# unclosed '[', so such matches would be counted as one.
set(nag_regex "\\$[0-9]+")
set(eval_only_regex "{[ \n]*\\[%eval[ \n]+[-#0-9.]+\\][ \n]*}")

# ---- --min-loss with --move-symbols std: hidden moves get no NAGs either ----

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan-mixed.txt
    ARGS --engine "${FAKE_ENGINE}" --min-loss 0.50 --move-symbols std loss_symbols.pgn)
assert_equal("${result}" "0" "apgn should succeed")

read_file(analysed "${WORK_DIR}/analyzed-loss_symbols.pgn")
assert_not_matches("${analysed}" "1\\.[ \n]+e4[ \n]+\\$" "1. e4 is under the minimum loss: no NAG, not even \$1")
assert_not_matches("${analysed}" "1\\.[ \n]+e4[ \n]+{" "1. e4 is under the minimum loss: no comment")
assert_matches("${analysed}" "e5[ \n]+\\$6[ \n]+\\$10[ \n]+{[ \n]*CP:0[ \n]*}" "1... e5 keeps its NAGs and its score")
assert_matches("${analysed}" "Nc6[ \n]+\\$4[ \n]+\\$18[ \n]+{[ \n]*CP:-400[ \n]*}[ \n]+\\(2\\.\\.\\.[ \n]+Nf6[ \n]+{[ \n]*CP:-100[ \n]*}"
    "2... Nc6 keeps its NAGs, its score and its variation")
count_matches(nags "${analysed}" "${nag_regex}")
assert_equal("${nags}" "4" "only the two annotated moves carry NAGs")

# ---- --stop-at-score with --eval-format std: tags up to the stop, nothing after ----

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan-stop.txt
    ARGS --engine "${FAKE_ENGINE}" --stop-at-score 2.00 --eval-format std stop_eval.pgn)
assert_equal("${result}" "0" "apgn should succeed")
assert_contains("${output}" "+2.14" "the stop message names the score")
assert_contains("${output}" "on move 2, skipping the rest of this game" "the stop message names the move")
assert_not_contains("${output}" "no plan for ply" "the engine must not be asked about the moves after the stop")

read_file(analysed "${WORK_DIR}/analyzed-stop_eval.pgn")
assert_matches("${analysed}" "1\\.[ \n]+e4[ \n]+{[ \n]*\\[%eval[ \n]+0\\.25\\]" "1. e4 has its eval tag")
assert_matches("${analysed}" "e5[ \n]+{[ \n]*\\[%eval[ \n]+0\\.30\\]" "1... e5 has its eval tag, from white's point of view")
assert_matches("${analysed}" "Bc4[ \n]+{[ \n]*\\[%eval[ \n]+2\\.14\\]" "the move that reached the score keeps its eval tag")
assert_not_matches("${analysed}" "Nc6[ \n]+{" "2... Nc6 comes after the stop: no annotation")
assert_not_contains("${analysed}" "CP:" "--eval-format std writes no CP: text")
count_matches(tags "${analysed}" "%eval")
assert_equal("${tags}" "6" "three analysed moves and their three variations carry eval tags")

# ---- --min-loss with --stop-at-score: a hidden move can still stop the analysis ----

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan-loss-stop.txt
    ARGS --engine "${FAKE_ENGINE}" --min-loss 0.50 --stop-at-score 2.00 loss_stop.pgn)
assert_equal("${result}" "0" "apgn should succeed")
assert_contains("${output}" "on move 2, skipping the rest of this game" "the best move at +2.14 still stops the analysis")
assert_not_contains("${output}" "no plan for ply" "the engine must not be asked about the moves after the stop")

read_file(analysed "${WORK_DIR}/analyzed-loss_stop.pgn")
count_matches(comments "${analysed}" "[A-Z]+[ \n]+(CP|MATE_IN):-?[0-9]+[ \n]*}")
assert_equal("${comments}" "1" "only 1... e5 loses enough to be annotated")
assert_matches("${analysed}" "e5[ \n]+{[ \n]*inaccurate" "1... e5 is annotated")
assert_not_matches("${analysed}" "Bc4[ \n]+{" "2. Bc4 is under the minimum loss: not annotated, although it stopped the analysis")

# ---- --move-symbols std with --eval-format std: no apgn wording anywhere ----

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan-mixed.txt
    ARGS --engine "${FAKE_ENGINE}" --move-symbols std --eval-format std standard.pgn)
assert_equal("${result}" "0" "apgn should succeed")
assert_not_contains("${output}" "Unknown" "pgn-extract must accept the output")

read_file(analysed "${WORK_DIR}/analyzed-standard.pgn")
string(REGEX REPLACE "\\[[A-Za-z]+ \"[^\"]*\"\\]\n" "" movetext "${analysed}") # the moves, without the pgn tags
assert_contains("${movetext}" "Qxf7#" "the move text must not be empty")
foreach(word CP: MATE_IN: "was the best move" "followed by" brilliant excellent good inaccurate mistake blunder
        missed accurate questionable WINNING ADVANTAGE UNCERTAIN DISADVANTAGE LOSING)
    assert_not_contains("${movetext}" "${word}" "fully standard output must not contain apgn's '${word}'")
endforeach()
count_matches(comments "${movetext}" "{")
assert_equal("${comments}" "14" "seven moves and seven variations are commented")
string(REGEX REPLACE "${eval_only_regex}" "" without_tags "${movetext}")
assert_not_contains("${without_tags}" "{" "every comment holds nothing but an eval tag")
assert_matches("${movetext}" "1\\.[ \n]+e4[ \n]+\\$1[ \n]+\\$10[ \n]+{[ \n]*\\[%eval[ \n]+0\\.30\\][ \n]*}[ \n]+\\(1\\.[ \n]+d4[ \n]+{[ \n]*\\[%eval[ \n]+0\\.30\\][ \n]*}"
    "1. e4 \$1 \$10 { [%eval 0.30] } (1. d4 { [%eval 0.30] })")

# ---- all four together ----

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan-all.txt
    ARGS --engine "${FAKE_ENGINE}" --min-loss 0.50 --stop-at-score 2.00 --move-symbols std --eval-format std all.pgn)
assert_equal("${result}" "0" "apgn should succeed with all four flags")
assert_contains("${output}" "+2.50" "the stop message names the score")
assert_not_contains("${output}" "no plan for ply" "the engine must not be asked about the moves after the stop")

read_file(analysed "${WORK_DIR}/analyzed-all.pgn")
string(REGEX REPLACE "\\[[A-Za-z]+ \"[^\"]*\"\\]\n" "" movetext "${analysed}")
assert_contains("${movetext}" "Qxf7#" "the move text must not be empty")
assert_not_matches("${movetext}" "1\\.[ \n]+e4[ \n]+[{$]" "1. e4 is under the minimum loss: nothing at all")
assert_matches("${movetext}" "e5[ \n]+\\$6[ \n]+\\$10[ \n]+{[ \n]*\\[%eval[ \n]+0\\.60\\][ \n]*}[ \n]+\\(1\\.\\.\\.[ \n]+d5[ \n]+{[ \n]*\\[%eval[ \n]+0\\.00\\][ \n]*}"
    "1... e5 \$6 \$10 { [%eval 0.60] } (1... d5 { [%eval 0.00] })")
assert_matches("${movetext}" "Bc4[ \n]+\\$2[ \n]+\\$18[ \n]+{[ \n]*\\[%eval[ \n]+2\\.50\\][ \n]*}[ \n]+\\(2\\.[ \n]+Nf3[ \n]+{[ \n]*\\[%eval[ \n]+3\\.50\\][ \n]*}"
    "2. Bc4 \$2 \$18 { [%eval 2.50] } (2. Nf3 { [%eval 3.50] })")
assert_not_matches("${movetext}" "Nc6[ \n]+[{$]" "2... Nc6 comes after the stop: nothing at all")
count_matches(nags "${movetext}" "${nag_regex}")
assert_equal("${nags}" "4" "only the two annotated moves carry NAGs")
