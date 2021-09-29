include(FetchContent)

## Like FetchContent_MakeAvailable, but adds EXCLUDE_FROM_ALL when invoking
## add_subdirectory().
macro(bmmidi_make_dep_available dep)
  FetchContent_GetProperties(${dep})

  if(NOT ${dep}_POPULATED)
    FetchContent_Populate(${dep})
    add_subdirectory("${${dep}_SOURCE_DIR}" "${${dep}_BINARY_DIR}" EXCLUDE_FROM_ALL)
  endif()
endmacro()
