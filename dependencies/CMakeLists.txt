include(BMMidiDeps)
include(FetchContent)

if(BMMidi_ENABLE_TESTING)
  FetchContent_Declare(googletest
      GIT_REPOSITORY https://github.com/google/googletest
      GIT_TAG e4717df71a4f45bf9f0ac88c6cd9846a0bc248dd)  # 2021-09-23
  bmmidi_make_dep_available(googletest)
endif()
