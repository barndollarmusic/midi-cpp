# SPDX-FileCopyrightText: 2022 Barndollar Music, Ltd.
#
# SPDX-License-Identifier: Apache-2.0

include(BMMidiTargets)

bmmidi_library(Lib
    bitops.hpp
    bmmidi.hpp
    channel.hpp
    cpp_features.hpp
    control.hpp
    data_value.hpp
    key_number.hpp
    msg_reference.hpp
    msg.hpp
    pitch_bend.hpp
    preset_number.hpp
    status.hpp
    sysex.cpp
    sysex.hpp
    timecode.cpp
    timecode.hpp
    timed.hpp)

if(BMMidi_ENABLE_TESTING)
  bmmidi_gtest(BitOpsTest bitops_test.cpp)
  target_link_libraries(BMMidi_BitOpsTest
      PRIVATE BMMidi::Lib)

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

  bmmidi_gtest(MsgReferenceTest msg_reference_test.cpp)
  target_link_libraries(BMMidi_MsgReferenceTest
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

  bmmidi_gtest(SysExTest sysex_test.cpp)
  target_link_libraries(BMMidi_SysExTest
      PRIVATE BMMidi::Lib)

  bmmidi_gtest(TimecodeTest timecode_test.cpp)
  target_link_libraries(BMMidi_TimecodeTest
      PRIVATE BMMidi::Lib)

  bmmidi_gtest(TimedTest timed_test.cpp)
  target_link_libraries(BMMidi_TimedTest
      PRIVATE BMMidi::Lib)
endif()
