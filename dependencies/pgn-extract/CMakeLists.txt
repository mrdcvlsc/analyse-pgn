cmake_minimum_required(VERSION 3.28)
project(pgn-extract C)

set(CMAKE_C_STANDARD 99)

set(BUILD_SHARED_LIBS OFF)

file(GLOB PGN_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

add_executable(pgn-extract ${PGN_SRCS})

target_compile_options(${PROJECT_NAME} PRIVATE -mtune=generic)

# ensure output folder exists in source tree and place binary there
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/pgn-extract)
set_target_properties(pgn-extract PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/pgn-extract
)

# copy license to output binary directory
add_custom_command(
    TARGET pgn-extract
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/COPYING" "$<TARGET_FILE_DIR:pgn-extract>/COPYING"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/copyright" "$<TARGET_FILE_DIR:pgn-extract>/copyright"
    COMMAND ${CMAKE_COMMAND} -E echo "https://www.cs.kent.ac.uk/people/staff/djb/pgn-extract/" > "$<TARGET_FILE_DIR:pgn-extract>/source_code.txt"
    COMMAND ${CMAKE_COMMAND} -E echo "https://github.com/kentdjb/pgn-extract/" >> "$<TARGET_FILE_DIR:pgn-extract>/source_code.txt"
    COMMENT "Copying license to pgn-extract bin directory"
)

# link math library on platforms that need it
find_library(M_LIB m)
if(M_LIB)
  target_link_libraries(pgn-extract PRIVATE ${M_LIB})
endif()

if(MSVC)
  # windows specific warnings/switches can go here if needed
else()
  target_link_libraries(pgn-extract PRIVATE -static-libgcc)
endif()
