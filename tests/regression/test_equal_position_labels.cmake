# Bug: the "losing" labels (accurate !!!, accurate !!, questionable ?) are
# documented as "the player is still in a losing position", but they are
# chosen whenever the score is <= 0. Playing the best move in a level
# position (CP:0, or e.g. CP:-30 for black) is labelled as if losing.
# Losing labels should only be used when the position is at a disadvantage
# (score below -115, i.e. DISADVANTAGE or LOSING).

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

# <ply> <engine best move> <best move score> <played move score>
write_file("${WORK_DIR}/plan.txt" [=[
# 1. e4: best move, level position
0 e2e4 cp:0 cp:0
# 1... e5: best move, black slightly worse but not losing
1 e7e5 cp:-30 cp:-30
# 2. Bc4: best move in a losing position
2 f1c4 cp:-200 cp:-200
# 2... Nc6: 0.20 worse than the best move in a level position
3 g8f6 cp:-5 cp:-25
4 d1h5 cp:200 cp:200
5 g8f6 cp:-100 cp:-100
6 h5f7 cp:50 cp:50
]=])

apgn_run(result output
    ENV FAKE_ENGINE_PLAN=${WORK_DIR}/plan.txt
    ARGS --engine "${FAKE_ENGINE}" game.pgn)
assert_equal("${result}" "0" "apgn should succeed")

read_file(analysed "${WORK_DIR}/analyzed-game.pgn")
assert_matches("${analysed}" "e4[ \n]+{[ \n]*brilliant !!![ \n]+UNCERTAIN" "best move at CP:0 is not a losing position")
assert_matches("${analysed}" "e5[ \n]+{[ \n]*brilliant !!![ \n]+UNCERTAIN" "best move at CP:-30 is not a losing position")
assert_matches("${analysed}" "Bc4[ \n]+{[ \n]*accurate !!![ \n]+LOSING" "best move at CP:-200 is still losing")
assert_matches("${analysed}" "Nc6[ \n]+{[ \n]*good ![ \n]+UNCERTAIN" "a good move in a level position")
assert_matches("${analysed}" "Nf6[ \n]+{[ \n]*brilliant !!![ \n]+UNCERTAIN" "best move at CP:-100 is not a losing position")
