# Bug: a pgn file without any complete game (e.g. a truncated download with
# no result token) silently produces empty analysed files and "success"
# messages. apgn should warn that no games were found and skip the file.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

write_file("${WORK_DIR}/truncated.pgn" [=[[Event "Truncated"]
[White "A"]
[Black "B"]

1. e4 e5 2. Nf3
]=])

apgn_run(result output ARGS ${APGN_FAST_ARGS} truncated.pgn)
assert_contains("${output}" "no games found" "an empty/truncated pgn file must be reported")
assert_not_exists("${WORK_DIR}/analyzed-truncated.pgn" "no analysed pgn should be written when there are no games")
assert_not_exists("${WORK_DIR}/analyzed-stats-truncated.json" "no stats should be written when there are no games")
