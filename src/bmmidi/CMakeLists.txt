include(BMMidiTargets)

bmmidi_header_library(Lib
    bmmidi.hpp
    channel.hpp
    key_number.hpp
    preset_number.hpp)

if(BMMidi_ENABLE_TESTING)
  bmmidi_gtest(ChannelTest channel_test.cpp)
  target_link_libraries(BMMidi_ChannelTest
      PRIVATE BMMidi::Lib)

  bmmidi_gtest(KeyNumberTest key_number_test.cpp)
  target_link_libraries(BMMidi_KeyNumberTest
      PRIVATE BMMidi::Lib)

  bmmidi_gtest(PresetNumberTest preset_number_test.cpp)
  target_link_libraries(BMMidi_PresetNumberTest
      PRIVATE BMMidi::Lib)
endif()
