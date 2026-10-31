# Bug: `apgn --help` describes --engine as "the directory location with the
# filename of your pgn file" and gives the default engine path as
# ./bin/engine/... while it is actually <apgn folder>/bin/engines/...

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

apgn_run(result output ARGS --help)
assert_equal("${result}" "0" "apgn --help should succeed")
assert_not_contains("${output}" "filename of your pgn file" "--engine is not a pgn file")
assert_contains("${output}" "chess engine executable" "--engine description")
assert_contains("${output}" "bin/engines/stockfish" "default engine path")
