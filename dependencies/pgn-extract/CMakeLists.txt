cmake_minimum_required(VERSION 3.30)
project(pgn-extract C)

set(CMAKE_C_STANDARD 99)

file(GLOB PGN_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

add_executable(pgn-extract ${PGN_SRCS})

# Ensure output folder exists in source tree and place binary there
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/pgn-extract)
set_target_properties(pgn-extract PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/pgn-extract
)

# Link math library on platforms that need it
find_library(M_LIB m)
if(M_LIB)
  target_link_libraries(pgn-extract PRIVATE ${M_LIB})
endif()

if(MSVC)
  # Windows specific warnings/switches can go here if needed
else()
  target_compile_options(pgn-extract PRIVATE -static-libgcc)
endif()
