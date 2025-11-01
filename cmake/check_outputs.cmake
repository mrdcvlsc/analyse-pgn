set(files
  pgn_samples/analyzed-first.pgn
  pgn_samples/first.pgn
#   pgn_samples/analyzed-first.stats
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