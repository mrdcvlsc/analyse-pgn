# Bug: Boost is downloaded as a .7z archive. libarchive converts the UTF-16
# file names stored in .7z archives to the current locale, so under a plain
# C/POSIX locale (common in containers and minimal CI images) the very first
# configure fails with
#   "Pathname cannot be converted from UTF-16LE to current locale."
#
# The test extracts the Boost archive that the parent build downloaded, with
# LC_ALL=C, exactly like FetchContent does.

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

file(GLOB boost_archives
    "${BINARY_DIR}/_deps/boost-subbuild/boost-populate-prefix/src/boost-*-cmake.*")
list(FILTER boost_archives EXCLUDE REGEX "-stamp$")

if(NOT boost_archives)
    fail("no downloaded Boost archive under ${BINARY_DIR}/_deps/boost-subbuild "
         "(this test needs a build that downloaded Boost, not FETCHCONTENT_SOURCE_DIR_BOOST)")
endif()
list(GET boost_archives 0 boost_archive)

file(MAKE_DIRECTORY "${WORK_DIR}/extract")
execute_process(
    COMMAND ${CMAKE_COMMAND} -E env LC_ALL=C LANG=C ${CMAKE_COMMAND} -E tar xf "${boost_archive}"
    WORKING_DIRECTORY "${WORK_DIR}/extract"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE output
)
file(WRITE "${WORK_DIR}/extract.log" "archive: ${boost_archive}\nresult: ${result}\n${output}")
file(REMOVE_RECURSE "${WORK_DIR}/extract") # ~1 GB of Boost sources

assert_not_contains("${output}" "cannot be converted" "the Boost archive must extract under the C locale")
assert_equal("${result}" "0" "extracting ${boost_archive} with LC_ALL=C")
