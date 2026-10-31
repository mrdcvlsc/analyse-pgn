# Bug: score_comments() mislabels moves when forced mates are involved:
#  - a move that forces mate is labelled "missed" when the best-move search
#    only reported a centipawn score;
#  - delaying a forced mate longer than the engine's line gives "wtf ?!!";
#  - a faster mate than the engine's line gives "god-move !!!!".
# The last two are placeholder strings that end up in the PGN and break the
# move statistics.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

# <ply> <engine best move> <best move score> <played move score>
write_file("${WORK_DIR}/plan.txt" [=[
# 1. e4: forces mate in 3, the best-move search only saw +3.00
0 d2d4 cp:300 mate:3
# 1... e5: getting mated in 5 instead of the engine's mate in 2 is the best defence
1 d7d5 mate:-2 mate:-5
# 2. Bc4: mate in 2 is faster than the engine's mate in 4
2 g1f3 mate:4 mate:2
# 2... Nc6: black forces mate while the engine's line was getting mated
3 g8f6 mate:-3 mate:3
4 d1h5 cp:50 cp:50
5 g8f6 cp:50 cp:50
6 h5f7 cp:50 cp:50
]=])

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan.txt
    ARGS --engine "${FAKE_ENGINE}" game.pgn)
assert_equal("${result}" "0" "apgn should succeed")

read_file(analysed "${WORK_DIR}/analyzed-game.pgn")
assert_not_contains("${analysed}" "wtf" "placeholder label must not be written")
assert_not_contains("${analysed}" "god-move" "placeholder label must not be written")
assert_matches("${analysed}" "e4[ \n]+{[ \n]*brilliant !!!" "1. e4 forces mate: it is the best move, not a missed one")
assert_matches("${analysed}" "e5[ \n]+{[ \n]*accurate !!!" "1... e5 is the best defence against mate")
assert_matches("${analysed}" "Bc4[ \n]+{[ \n]*brilliant !!!" "2. Bc4 mates faster than the engine's line")
assert_matches("${analysed}" "Nc6[ \n]+{[ \n]*brilliant !!!" "2... Nc6 forces mate")

read_file(stats "${WORK_DIR}/analyzed-stats-game.json")
assert_not_contains("${stats}" "invalid argument" "every move must be counted under a real label")
