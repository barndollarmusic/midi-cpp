include(BMMidiTargets)

bmmidi_header_library(Lib
    bmmidi.hpp
    channel.hpp)

if(BMMidi_ENABLE_TESTING)
  bmmidi_gtest(ChannelTest channel_test.cpp)
  target_link_libraries(BMMidi_ChannelTest
      PRIVATE BMMidi::Lib)
endif()
