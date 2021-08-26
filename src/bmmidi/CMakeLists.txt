include(BMMidiTargets)

bmmidi_header_library(Lib
    bmmidi.hpp
    channel.hpp
    control.hpp
    data_value.hpp
    key_number.hpp
    msg.hpp
    pitch_bend.hpp
    preset_number.hpp
    status.hpp)

if(BMMidi_ENABLE_TESTING)
  bmmidi_gtest(ChannelTest channel_test.cpp)
  target_link_libraries(BMMidi_ChannelTest
      PRIVATE BMMidi::Lib)

  bmmidi_gtest(ControlTest control_test.cpp)
  target_link_libraries(BMMidi_ControlTest
      PRIVATE BMMidi::Lib)

  bmmidi_gtest(DataValueTest data_value_test.cpp)
  target_link_libraries(BMMidi_DataValueTest
      PRIVATE BMMidi::Lib)

  bmmidi_gtest(KeyNumberTest key_number_test.cpp)
  target_link_libraries(BMMidi_KeyNumberTest
      PRIVATE BMMidi::Lib)

  bmmidi_gtest(MsgTest msg_test.cpp)
  target_link_libraries(BMMidi_MsgTest
      PRIVATE BMMidi::Lib)

  bmmidi_gtest(PitchBendTest pitch_bend_test.cpp)
  target_link_libraries(BMMidi_PitchBendTest
      PRIVATE BMMidi::Lib)

  bmmidi_gtest(PresetNumberTest preset_number_test.cpp)
  target_link_libraries(BMMidi_PresetNumberTest
      PRIVATE BMMidi::Lib)

  bmmidi_gtest(StatusTest status_test.cpp)
  target_link_libraries(BMMidi_StatusTest
      PRIVATE BMMidi::Lib)
endif()
