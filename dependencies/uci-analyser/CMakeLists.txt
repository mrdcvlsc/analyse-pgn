cmake_minimum_required(VERSION 3.30)
project(uci-analyser CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB UCI_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(analyse ${UCI_SRCS})

# Ensure output folder exists in source tree and place binary there
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/analyse)
set_target_properties(analyse PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/analyse
)

if(NOT MSVC)
  target_compile_options(analyse PRIVATE -static-libgcc -static-libstdc++)
endif()
