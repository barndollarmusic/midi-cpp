cmake_minimum_required(VERSION 3.18)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

project(BMMidi
    VERSION 0.1
    DESCRIPTION "C++ MIDI utilities (direct or adapt another framework)"
    HOMEPAGE_URL "https://github.com/barndollarmusic/midi-cpp"
    LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL BMMidi_SOURCE_DIR)
  set(BMMIDI_IS_TOP_LEVEL_PROJECT YES)
  set(bmMidiTestingDefault ON)
else()
  set(bmMidiTestingDefault OFF)
endif()
  
option(BMMidi_ENABLE_TESTING "Enable testing for the BMMidi project"
    ${bmMidiTestingDefault})
if(BMMidi_ENABLE_TESTING)
  enable_testing()
endif()

include(BMMidiDefaults)

add_subdirectory(dependencies)
add_subdirectory(src)
