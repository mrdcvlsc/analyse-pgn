cmake_minimum_required(VERSION 3.30)
project(apgn LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# Respect previous repository layout: create output folders in the source tree
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/engines)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/analyse)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/pgn-extract)

# Subprojects
add_subdirectory(dependencies/pgn-extract)
add_subdirectory(dependencies/uci-analyser)

# Top-level executable
add_executable(apgn
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${CMAKE_SOURCE_DIR}/apgnFileSys.cpp
    ${CMAKE_SOURCE_DIR}/convert.cpp
    ${CMAKE_SOURCE_DIR}/winProcRun.cpp
)
target_compile_features(apgn PRIVATE cxx_std_17)

# Put the top-level executable in the repository root (matches previous Makefile)
set_target_properties(apgn PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

enable_testing()
# Basic functional test: run apgn against the provided sample PGN
add_test(NAME run_apgn_sample
  COMMAND $<TARGET_FILE:apgn> ${CMAKE_SOURCE_DIR}/pgn_samples/first.pgn -color W
)

# Install rules (used by CPack/package)
install(TARGETS apgn RUNTIME DESTINATION ".")
install(TARGETS analyse RUNTIME DESTINATION "bin/analyse")
install(TARGETS pgn-extract RUNTIME DESTINATION "bin/pgn-extract")

# If a bundled engine exists in source tree, include it in the package
set(DEFAULT_CHESS_ENGINE "${CMAKE_SOURCE_DIR}/bin/engines/stockfish")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(DEFAULT_CHESS_ENGINE "${CMAKE_SOURCE_DIR}/bin/engines/stockfish.exe")
endif()

if(EXISTS ${DEFAULT_CHESS_ENGINE})
  install(FILES ${DEFAULT_CHESS_ENGINE} DESTINATION "bin/engines")
endif()

# CPack packaging configuration (produce a deterministic ZIP)
set(CPACK_GENERATOR "ZIP")
string(TOLOWER "${CMAKE_BUILD_TYPE}" _build_type)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CPACK_PACKAGE_FILE_NAME "apgn-${CMAKE_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}")
set(CPACK_STRIP_FILES FALSE)
include(CPack)
