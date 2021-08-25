include(GNUInstallDirs)

## Adds header-only library with BMMidi project defaults.
##
## Creates target named BMMidi_${name} and alias of
## BMMidi::${name}.
##
## All remaining arguments are passed as sources to target_sources().
function(bmmidi_header_library name)
  add_library(BMMidi_${name} INTERFACE)
  add_library(BMMidi::${name} ALIAS BMMidi_${name})
  set_target_properties(BMMidi_${name} PROPERTIES
      EXPORT_NAME ${name})

  target_sources(BMMidi_${name} INTERFACE ${ARGN})

  # Always include the src/ dir as a base include path.
  target_include_directories(BMMidi_${name}
      INTERFACE ${BMMidi_SOURCE_DIR}/src)

  target_compile_features(BMMidi_${name} INTERFACE cxx_std_14)
endfunction()

## Configures target compile options to enable this project's standard set of
## warnings for the BMMidi_${name} target.
function(bmmidi_enable_warnings name)
  # Enable strict warnings.
  if (MSVC)
    target_compile_options(BMMidi_${name}
        PRIVATE /W4)
  else()
    target_compile_options(BMMidi_${name}
        PRIVATE -Wall -Wextra -pedantic)
  endif()
endfunction()

## Adds library with BMMidi project defaults.
##
## Creates target named BMMidi_${name} and alias of
## BMMidi::${name}.
##
## All remaining arguments are passed to add_library().
function(bmmidi_library name)
  add_library(BMMidi_${name} ${ARGN})
  add_library(BMMidi::${name} ALIAS BMMidi_${name})
  set_target_properties(BMMidi_${name} PROPERTIES
      OUTPUT_NAME ${name}
      EXPORT_NAME ${name})

  # Always include the src/ dir as a base include path.
  target_include_directories(BMMidi_${name}
      PUBLIC ${BMMidi_SOURCE_DIR}/src)

  target_compile_features(BMMidi_${name} PUBLIC cxx_std_14)
  bmmidi_enable_warnings(${name})

  # TODO: Configure installation, if necessary.
endfunction()

## If testing for this project is enabled, adds test executable and matching
## CTest test with BMMidi project defaults. Creates executable named
## BMMidi_${name}.
##
## All remaining arguments are passed to add_executable().
function(bmmidi_gtest name)
  if(NOT BMMidi_ENABLE_TESTING)
    # Project testing off, so bmmidi_gtest() should never have been invoked.
    message(FATAL_ERROR "Must guard test targets with BMMidi_ENABLE_TESTING")
  endif()

  include(GoogleTest)

  add_executable(BMMidi_${name} ${ARGN})

  # Always include the src/ dir as a base include path.
  target_include_directories(BMMidi_${name}
      PUBLIC ${BMMidi_SOURCE_DIR}/src)

  target_compile_features(BMMidi_${name} PUBLIC cxx_std_14)
  bmmidi_enable_warnings(${name})

  target_link_libraries(BMMidi_${name}
      PRIVATE gtest gmock gtest_main)
  gtest_discover_tests(BMMidi_${name}
      WORKING_DIRECTORY ${BMMidi_BINARY_DIR}/stage/${CMAKE_INSTALL_BINDIR})
endfunction()
