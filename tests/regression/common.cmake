# Shared helpers for the regression tests in this directory.
#
# Every tests/regression/test_*.cmake script is registered with CTest by the
# top-level CMakeLists.txt and run as `cmake -P <script>` with:
#
#   APGN           path to the apgn executable under test
#   SOURCE_DIR     repository root
#   BINARY_DIR     top-level CMake build directory
#   WORK_DIR       scratch directory for this test (recreated on every run)
#   FAKE_ENGINE    path to tests/regression/fake_engine.py
#   BOOST_SOURCE_DIR, GLAZE_SOURCE_DIR, GENERATOR, C_COMPILER, CXX_COMPILER
#                  values from the parent build, for tests that run CMake
#
# Scripts named test_posix_*.cmake are only registered on non-Windows hosts
# (they use the Python fake engine, `make`, or POSIX locales).

cmake_minimum_required(VERSION 3.28)

file(REMOVE_RECURSE "${WORK_DIR}")
file(MAKE_DIRECTORY "${WORK_DIR}")

set(APGN_RUN_COUNT 0)

# Fast default engine settings so tests finish quickly and deterministically.
set(APGN_FAST_ARGS --threads 1 --hash-size 16 --depth 6)

# apgn_run(<result_var> <output_var> [ENV <k=v>...] ARGS <arg>...)
#
# Runs apgn inside WORK_DIR with stdout and stderr merged into <output_var>.
# The output is also written to WORK_DIR/apgn-run-<n>.log for debugging.
function(apgn_run result_var output_var)
    cmake_parse_arguments(RUN "" "" "ENV;ARGS" ${ARGN})

    math(EXPR run_number "${APGN_RUN_COUNT} + 1")
    set(APGN_RUN_COUNT ${run_number} PARENT_SCOPE)

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E env ${RUN_ENV} "${APGN}" ${RUN_ARGS}
        WORKING_DIRECTORY "${WORK_DIR}"
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE output
        TIMEOUT 600
    )

    file(WRITE "${WORK_DIR}/apgn-run-${run_number}.log"
        "command: ${APGN} ${RUN_ARGS}\nenv: ${RUN_ENV}\nresult: ${result}\n\n${output}")

    set(${result_var} "${result}" PARENT_SCOPE)
    set(${output_var} "${output}" PARENT_SCOPE)
endfunction()

# write_file(<path> <content>) - write a test input file.
function(write_file path content)
    file(WRITE "${path}" "${content}")
endfunction()

# read_file(<var> <path>) - read a file, failing the test if it is missing.
function(read_file var path)
    if(NOT EXISTS "${path}")
        fail("expected file to exist: ${path}")
    endif()
    file(READ "${path}" content)
    set(${var} "${content}" PARENT_SCOPE)
endfunction()

function(fail message)
    message(FATAL_ERROR "\nREGRESSION TEST FAILED: ${message}\n(logs in ${WORK_DIR})\n")
endfunction()

function(assert_contains haystack needle what)
    string(FIND "${haystack}" "${needle}" position)
    if(position EQUAL -1)
        fail("${what}\n  expected to find: ${needle}\n  in:\n${haystack}")
    endif()
endfunction()

function(assert_not_contains haystack needle what)
    string(FIND "${haystack}" "${needle}" position)
    if(NOT position EQUAL -1)
        fail("${what}\n  did not expect to find: ${needle}\n  in:\n${haystack}")
    endif()
endfunction()

function(assert_matches haystack regex what)
    if(NOT "${haystack}" MATCHES "${regex}")
        fail("${what}\n  expected a match for regex: ${regex}\n  in:\n${haystack}")
    endif()
endfunction()

function(assert_not_matches haystack regex what)
    if("${haystack}" MATCHES "${regex}")
        fail("${what}\n  did not expect a match for regex: ${regex}\n  in:\n${haystack}")
    endif()
endfunction()

function(assert_equal actual expected what)
    if(NOT "${actual}" STREQUAL "${expected}")
        fail("${what}\n  expected: ${expected}\n  actual:   ${actual}")
    endif()
endfunction()

function(assert_exists path what)
    if(NOT EXISTS "${path}")
        fail("${what}\n  missing: ${path}")
    endif()
endfunction()

function(assert_not_exists path what)
    if(EXISTS "${path}")
        fail("${what}\n  unexpectedly exists: ${path}")
    endif()
endfunction()

# count_matches(<var> <text> <regex>) - number of non-overlapping matches.
function(count_matches var text regex)
    string(REGEX MATCHALL "${regex}" matches "${text}")
    list(LENGTH matches count)
    set(${var} ${count} PARENT_SCOPE)
endfunction()

# The move-quality comment apgn attaches to an analysed move, e.g.
# "{ brilliant !!! WINNING CP:276 }" - used to count analysed moves.
set(APGN_ANALYSIS_COMMENT_REGEX "[A-Z]+ (CP|MATE_IN):-?[0-9]+ }")

# A short game ending in mate (7 plies + result token).
set(SCHOLARS_MATE_PGN [=[[Event "Scholar's mate"]
[Site "?"]
[Date "????.??.??"]
[Round "?"]
[White "A"]
[Black "B"]
[Result "1-0"]

1. e4 e5 2. Bc4 Nc6 3. Qh5 Nf6 4. Qxf7# 1-0
]=])

# A game with a promotion (11 plies).
set(PROMOTION_PGN [=[[Event "Promotion"]
[Site "?"]
[Date "????.??.??"]
[Round "?"]
[White "A"]
[Black "B"]
[Result "*"]

1. e4 d5 2. exd5 c6 3. dxc6 Nf6 4. cxb7 Nbd7 5. bxa8=Q e5 6. Qxa7 *
]=])
