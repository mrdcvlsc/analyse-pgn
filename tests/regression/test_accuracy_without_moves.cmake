# Bug: when one side is not analysed (--player White/Black), its accuracy is
# computed as 0/0 = NaN and the stats JSON contains "accuracy": null next to
# empty move lists. Only analysed players should be reported.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

apgn_run(result output ARGS ${APGN_FAST_ARGS} --player Black "${WORK_DIR}/game.pgn")
assert_equal("${result}" "0" "apgn should succeed")

read_file(stats "${WORK_DIR}/analyzed-stats-game.json")
assert_not_contains("${stats}" "null" "the stats must not contain null (NaN) values")

string(JSON player_count LENGTH "${stats}" games 0 player_stats)
string(JSON color GET "${stats}" games 0 player_stats 0 player_color)
string(JSON accuracy GET "${stats}" games 0 player_stats 0 accuracy)
assert_equal("${player_count}" "1" "only the analysed player is reported")
assert_equal("${color}" "black" "the reported player is black")
if(NOT accuracy GREATER_EQUAL 0 OR NOT accuracy LESS_EQUAL 1)
    fail("black's accuracy must be a number between 0 and 1, got: ${accuracy}")
endif()
