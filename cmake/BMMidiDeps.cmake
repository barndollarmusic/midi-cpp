# SPDX-FileCopyrightText: 2022 Barndollar Music, Ltd.
#
# SPDX-License-Identifier: Apache-2.0

include(FetchContent)

## Like FetchContent_MakeAvailable, but adds EXCLUDE_FROM_ALL when invoking
## add_subdirectory(). Accepts SOURCE_SUBDIR argument (which must be repeated,
## since we don't want to access private FetchContent() state).
macro(bmmidi_make_dep_available dep)
  cmake_parse_arguments(ARG "" "SOURCE_SUBDIR" "" ${ARGN})

  string(TOLOWER "${dep}" lowerCaseDep)
  FetchContent_GetProperties(${dep})

  if(NOT ${lowerCaseDep}_POPULATED)
    FetchContent_Populate(${dep})

    set(srcDir "${${lowerCaseDep}_SOURCE_DIR}")
    if(NOT "${ARG_SOURCE_SUBDIR}" STREQUAL "")
      string(APPEND srcDir "/${ARG_SOURCE_SUBDIR}")
    endif()

    add_subdirectory("${srcDir}" "${${lowerCaseDep}_BINARY_DIR}"
        EXCLUDE_FROM_ALL)
  endif()
endmacro()
