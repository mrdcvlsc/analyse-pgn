# warnings_as_errors.cmake
# usage:
#   include("cmake/warnings_as_errors.cmake")
#   add_library(foo ...)
#   set_warnings_as_errors(foo)
#

option(TREAT_SHADOW_AND_UNUSED_AS_ERRORS "Treat shadow/unused warnings as errors" ON)

function(set_warnings_as_errors_for_own_sources target)
  # get list of sources that were added to target
  get_target_property(_srcs ${target} SOURCES)
  if(NOT _srcs)
    return()
  endif()

  if(MSVC)
    set(WERR_FLAGS /we4456 /we4457 /we4458 /we4101 /we4189)
  else()
    set(WERR_FLAGS
      -Wshadow -Wunused -Werror=shadow -Werror=unused
    )
  endif()

  foreach(src IN LISTS _srcs)
    # only operate on files that are inside your source tree
    if(src MATCHES "^${CMAKE_SOURCE_DIR}/src/")
      get_filename_component(src_abs "${src}" ABSOLUTE)
      set_source_files_properties("${src_abs}"
        PROPERTIES
          COMPILE_OPTIONS "${WERR_FLAGS}"
      )
    endif()
  endforeach()
endfunction()


function(set_warnings_as_errors target)
  if (NOT TREAT_SHADOW_AND_UNUSED_AS_ERRORS)
    return()
  endif()

  if (MSVC)
    target_compile_options(${target} PRIVATE
      /we4456   # treat 'hides local' as error
      /we4457   # treat 'hides parameter' as error
      /we4458   # treat 'hides class member' as error
      /we4101   # treat unreferenced local as error
      /we4189   # treat initialized-but-unreferenced as error
    )
  else()
    # GCC / Clang / AppleClang
    # use -Wshadow plus fine-grained -Wunused-* and turn them into errors.
    #
    # apply only when compiling C/C++ (generator expressions), so the flags won't
    # affect other languages accidentally.

    target_compile_options(${target} PRIVATE
      # For C files (GNU/Clang)
      $<$<AND:$<COMPILE_LANGUAGE:C>,$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:AppleClang>>>:
        -Wshadow -Werror=shadow -Wunused -Werror=unused>

      # For C++ files (GNU/Clang)
      $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>>:
        -Wshadow -Werror=shadow -Wunused -Werror=unused>
    )
  endif()
endfunction()

# Convenience: apply to all targets created after including this file.
# Example: call set_warnings_for_all_targets() after you add targets that exist now.
function(set_warnings_for_all_targets)
  get_property(_all TARGETS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS)
  foreach(t IN LISTS _all)
    if (NOT TARGET ${t})
      continue()
    endif()
    if (NOT ("$<TARGET_PROPERTY:${t},INTERFACE_LINK_LIBRARIES>" STREQUAL ""))
      # ignore
    endif()
    set_warnings_as_errors(${t})
  endforeach()
endfunction()
