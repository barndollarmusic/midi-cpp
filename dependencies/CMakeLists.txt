include(FetchContent)

if(BMMidi_ENABLE_TESTING)
  FetchContent_Declare(googletest
      GIT_REPOSITORY https://github.com/google/googletest
      GIT_TAG 40dfd4b775a66979ad1bd19321cdfd0feadfea27)  # 2021-09-16
  FetchContent_MakeAvailable(googletest)
endif()
