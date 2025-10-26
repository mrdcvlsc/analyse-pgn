# Cross-platform checks performed by CI after running tests
if(NOT DEFINED PROJECT_SOURCE_DIR)
  message(FATAL_ERROR "PROJECT_SOURCE_DIR must be passed as -DPROJECT_SOURCE_DIR=<path>")
endif()

set(files
  "${PROJECT_SOURCE_DIR}/pgn_samples/first.analyzed.pgn"
  "${PROJECT_SOURCE_DIR}/pgn_samples/first.pgn"
  "${PROJECT_SOURCE_DIR}/pgn_samples/first.stats.txt"
)

set(missing)
foreach(f ${files})
  if(NOT EXISTS "${f}")
    list(APPEND missing "${f}")
  else()
    file(SIZE "${f}" size)
    if(size EQUAL 0)
      list(APPEND missing "${f} (empty)")
    endif()
  endif()
endforeach()

if(missing)
  message(FATAL_ERROR "One or more files are missing or empty: ${missing}")
else()
  message(STATUS "All expected pgn_samples files present and non-empty")
endif()
