include(FetchContent)

if(BMMidi_ENABLE_TESTING)
  FetchContent_Declare(googletest
      GIT_REPOSITORY https://github.com/google/googletest
      GIT_TAG ff21b36e1e858bf9274ed2bbf4921e415363c393)  # 2021-08-24
  FetchContent_MakeAvailable(googletest)
endif()
