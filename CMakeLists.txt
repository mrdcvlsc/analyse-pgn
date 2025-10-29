cmake_minimum_required(VERSION 3.28)
project(apgn LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

set(BOOST_INCLUDE_LIBRARIES system filesystem process asio)
set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)

FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.89.0/boost-1.89.0-cmake.7z
)

FetchContent_MakeAvailable(Boost)

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/pgn-extract)
add_subdirectory(${CMAKE_SOURCE_DIR}/dependencies/pgn-extract)

add_executable(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/src/apgn.cpp
    ${CMAKE_SOURCE_DIR}/src/chess_games.cpp
    ${CMAKE_SOURCE_DIR}/src/get_exe_dir.cpp
    ${CMAKE_SOURCE_DIR}/src/logger.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC "${Boost_SOURCE_DIR}")
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system Boost::filesystem Boost::process)

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

enable_testing()

add_test(NAME run_apgn_sample
  COMMAND $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_SOURCE_DIR}/pgn_samples/first.pgn -color W -depth 11
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ".")
install(TARGETS pgn-extract RUNTIME DESTINATION "${CMAKE_SOURCE_DIR}/bin/pgn-extract")

set(DEFAULT_CHESS_ENGINE "${CMAKE_SOURCE_DIR}/bin/engines/stockfish")

set(CPACK_GENERATOR "ZIP")
string(TOLOWER "${CMAKE_BUILD_TYPE}" _build_type)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CPACK_PACKAGE_FILE_NAME "apgn-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}")
set(CPACK_STRIP_FILES FALSE)
include(CPack)

# ===================================
# old cmake
# ===================================

# cmake_minimum_required(VERSION 3.30)
# project(apgn LANGUAGES C CXX)

# set(CMAKE_C_STANDARD 99)
# set(CMAKE_CXX_STANDARD 17)

# include(FetchContent)

# FetchContent_Declare(
#   Boost
#   URL https://github.com/boostorg/boost/releases/download/boost-1.83.0/boost-1.83.0.tar.gz
# )

# FetchContent_MakeAvailable(Boost)

# # Respect previous repository layout: create output folders in the source tree
# file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/engines)
# file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/analyse)
# file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/pgn-extract)

# # Subprojects
# add_subdirectory(dependencies/pgn-extract)
# add_subdirectory(dependencies/uci-analyser)

# # Top-level executable
# add_executable(apgn
#     ${CMAKE_SOURCE_DIR}/main.cpp
#     ${CMAKE_SOURCE_DIR}/apgnFileSys.cpp
#     ${CMAKE_SOURCE_DIR}/convert.cpp
# )
# target_compile_features(apgn PRIVATE cxx_std_17)
# target_include_directories(apgn PRIVATE ${Boost_SOURCE_DIR})

# add_executable(apgn
#     ${CMAKE_SOURCE_DIR}/main.cpp
#     ${CMAKE_SOURCE_DIR}/apgnFileSys.cpp
#     ${CMAKE_SOURCE_DIR}/convert.cpp
# )
# target_compile_features(apgn PRIVATE cxx_std_17)
# target_include_directories(apgn PRIVATE ${Boost_SOURCE_DIR})


# ## Note: Boost was fetched as source with FetchContent. We expose the source
# ## directory on the include path so header-only components (like Boost.Process)
# ## can be used. Linking against built Boost targets is not performed here.

# # Put the top-level executable in the repository root (matches previous Makefile)
# set_target_properties(apgn PROPERTIES
#   RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
# )

# enable_testing()
# # Basic functional test: run apgn against the provided sample PGN
# add_test(NAME run_apgn_sample
#   COMMAND $<TARGET_FILE:apgn> ${CMAKE_SOURCE_DIR}/pgn_samples/first.pgn -color W -depth 11
# )

# # Install rules (used by CPack/package)
# install(TARGETS apgn RUNTIME DESTINATION ".")
# install(TARGETS analyse RUNTIME DESTINATION "bin/analyse")
# install(TARGETS pgn-extract RUNTIME DESTINATION "bin/pgn-extract")

# # If a bundled engine exists in source tree, include it in the package
# set(DEFAULT_CHESS_ENGINE "${CMAKE_SOURCE_DIR}/bin/engines/stockfish")

# if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
#   set(DEFAULT_CHESS_ENGINE "${CMAKE_SOURCE_DIR}/bin/engines/stockfish.exe")
# endif()

# if(EXISTS ${DEFAULT_CHESS_ENGINE})
#   install(FILES ${DEFAULT_CHESS_ENGINE} DESTINATION "bin/engines")
# endif()

# # CPack packaging configuration (produce a deterministic ZIP)
# set(CPACK_GENERATOR "ZIP")
# string(TOLOWER "${CMAKE_BUILD_TYPE}" _build_type)
# if(NOT CMAKE_BUILD_TYPE)
#   set(CMAKE_BUILD_TYPE Release)
# endif()
# set(CPACK_PACKAGE_FILE_NAME "apgn-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}")
# set(CPACK_STRIP_FILES FALSE)
# include(CPack)
