include(FetchContent)

## Like FetchContent_MakeAvailable, but adds EXCLUDE_FROM_ALL when invoking
## add_subdirectory().
macro(bmmidi_make_dep_available dep)
  string(TOLOWER "${dep}" lowerCaseDep)
  FetchContent_GetProperties(${dep})

  if(NOT ${lowerCaseDep}_POPULATED)
    FetchContent_Populate(${dep})
    add_subdirectory("${${lowerCaseDep}_SOURCE_DIR}" "${${lowerCaseDep}_BINARY_DIR}"
        EXCLUDE_FROM_ALL)
  endif()
endmacro()
