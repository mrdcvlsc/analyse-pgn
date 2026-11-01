# Bug: numeric flag values are only checked for being digits. Values that
# make no sense (--depth 0, --hash-size 0) are passed on to the engine, and
# huge values overflow `int` (std::atoi on more than 2147483647 is undefined;
# --analyse-until-move 1500000000 overflows once converted to plies), which
# silently skips the whole analysis. Values outside a sensible range must be
# rejected with an error.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/game.pgn" "${SCHOLARS_MATE_PGN}")

# flag, rejected value
set(rejected_values
    "--depth|0"
    "--depth|101"
    "--threads|0"
    "--threads|99999999999999999999999"
    "--hash-size|0"
    "--analyse-start-on-move|10001"
    "--analyse-until-move|0"
    "--analyse-until-move|1500000000"
)

foreach(entry ${rejected_values})
    string(REPLACE "|" ";" flag_and_value "${entry}")
    list(GET flag_and_value 0 flag)
    list(GET flag_and_value 1 value)

    apgn_run(result output ARGS ${flag} ${value} game.pgn)
    assert_equal("${result}" "1" "exit status for ${flag} ${value}")
    assert_contains("${output}" "must be between" "${flag} ${value} must be rejected with its allowed range")
    assert_not_exists("${WORK_DIR}/analyzed-game.pgn" "nothing should be analysed for ${flag} ${value}")
endforeach()

# the limits themselves are accepted
apgn_run(result output ARGS --depth 1 --threads 1 --hash-size 1 --analyse-start-on-move 0 --analyse-until-move 1 game.pgn)
assert_equal("${result}" "0" "values at the lower limits are valid")
assert_exists("${WORK_DIR}/analyzed-game.pgn" "the game must be analysed with valid values")
