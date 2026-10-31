# Bug: configuring the project without -DCMAKE_BUILD_TYPE fails:
#   if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
# expands to `if( STREQUAL "Debug")` when the variable is empty.
#
# The test configures the repository again in a scratch directory, reusing the
# Boost and glaze sources already fetched by the parent build.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

execute_process(
    COMMAND ${CMAKE_COMMAND}
        -S "${SOURCE_DIR}" -B "${WORK_DIR}/build"
        -G "${GENERATOR}"
        "-DCMAKE_C_COMPILER=${C_COMPILER}"
        "-DCMAKE_CXX_COMPILER=${CXX_COMPILER}"
        "-DFETCHCONTENT_SOURCE_DIR_BOOST=${BOOST_SOURCE_DIR}"
        "-DFETCHCONTENT_SOURCE_DIR_GLAZE=${GLAZE_SOURCE_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE output
)
file(WRITE "${WORK_DIR}/configure.log" "${output}")

assert_not_contains("${output}" "if given arguments" "CMakeLists.txt must handle an empty CMAKE_BUILD_TYPE")
assert_equal("${result}" "0" "configuring without CMAKE_BUILD_TYPE should succeed")
