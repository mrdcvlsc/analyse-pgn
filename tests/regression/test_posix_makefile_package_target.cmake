# Bug: `make package` runs `cmake --build ... --target package`, but the
# CMake project has no "package" target (the release archive target is
# called package_for_release), so `make package` always fails.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

file(GLOB old_archives "${SOURCE_DIR}/apgn-*.zip")
if(old_archives)
    file(REMOVE ${old_archives})
endif()

execute_process(
    COMMAND make -C "${SOURCE_DIR}" package "CMAKE_BUILD_DIR=${BINARY_DIR}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE output
)
file(WRITE "${WORK_DIR}/make-package.log" "${output}")

file(GLOB archives "${SOURCE_DIR}/apgn-*.zip")
if(archives)
    file(REMOVE ${archives}) # keep the source tree clean
endif()

assert_not_contains("${output}" "No rule to make target" "make package must use an existing CMake target")
assert_equal("${result}" "0" "make package should succeed")
if(NOT archives)
    fail("make package should create the apgn-<system>-<compiler>.zip release archive")
endif()
