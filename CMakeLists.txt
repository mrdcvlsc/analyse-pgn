cmake_minimum_required(VERSION 3.28)
project(apgn LANGUAGES C CXX)

set(VERBOSE 1)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BUILD_SHARED_LIBS OFF)

set(BOOST_ENABLE_CMAKE ON)
set(BOOST_INCLUDE_LIBRARIES system filesystem process asio)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)

include(cmake/warnings_as_errors.cmake)
include(FetchContent)

set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_SILENT OFF)

FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.89.0/boost-1.89.0-cmake.7z
)

FetchContent_MakeAvailable(Boost)

FetchContent_Declare(
  glaze
  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
  GIT_TAG v6.0.1
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glaze)

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/pgn-extract)
add_subdirectory(${CMAKE_SOURCE_DIR}/dependencies/pgn-extract)

add_executable(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/src/apgn.cpp
    ${CMAKE_SOURCE_DIR}/src/info/display_info.cpp
    ${CMAKE_SOURCE_DIR}/src/extract_args/checks.cpp
    ${CMAKE_SOURCE_DIR}/src/extract_args/extract_args.cpp
    ${CMAKE_SOURCE_DIR}/src/extract_args/get_defaults.cpp
    ${CMAKE_SOURCE_DIR}/src/analyse_game/analyse_game.cpp
    ${CMAKE_SOURCE_DIR}/src/analyse_game/get_ucigo_bestmove.cpp
    ${CMAKE_SOURCE_DIR}/src/analyse_game/score_comments.cpp
    ${CMAKE_SOURCE_DIR}/src/load_games/load_games.cpp
    ${CMAKE_SOURCE_DIR}/src/save_games/save_games.cpp
    ${CMAKE_SOURCE_DIR}/src/generate_stats/generate_stats.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/process_utils.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/get_exe_dir.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/logger.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_include_directories(${PROJECT_NAME} PUBLIC "${Boost_SOURCE_DIR}")
target_compile_options(${PROJECT_NAME} PRIVATE -mtune=generic)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Boost::system Boost::filesystem Boost::process
    glaze::glaze
    -static -static-libgcc -static-libstdc++
)
# set_warnings_as_errors_for_own_sources(${PROJECT_NAME})

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -fno-omit-frame-pointer)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

enable_testing()

add_test(NAME run_apgn_sample
  COMMAND $<TARGET_FILE:${PROJECT_NAME}> --player Black --threads 1 ${CMAKE_SOURCE_DIR}/pgn_samples/first.pgn --depth 4 --hash-size 200
)

get_filename_component(CXX_COMPILER_NAME ${CMAKE_CXX_COMPILER} NAME_WE)
string(TOLOWER "apgn-${CMAKE_SYSTEM_NAME}-${CXX_COMPILER_NAME}.zip" ZIP_OUTPUT_FILE)

message("==============================")
message("To be zipped:")
message("${CMAKE_SOURCE_DIR}/${ZIP_OUTPUT_FILE}")
message("${CMAKE_SOURCE_DIR}/apgn${CMAKE_EXECUTABLE_SUFFIX}")
message("${CMAKE_SOURCE_DIR}/bin")
message("${CMAKE_SOURCE_DIR}/pgn_samples")
message("==============================")

add_custom_target(package_for_release
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --blue "Creating archive for release..."
    COMMAND ${CMAKE_COMMAND} -E tar cf "${CMAKE_SOURCE_DIR}/${ZIP_OUTPUT_FILE}" --format=zip "${CMAKE_SOURCE_DIR}/bin" "${CMAKE_SOURCE_DIR}/pgn_samples" "${CMAKE_SOURCE_DIR}/apgn${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_SOURCE_DIR}/LICENSE"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Archiving build"
    DEPENDS ${PROJECT_NAME}
)
