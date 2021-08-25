#include "bmmidi/status.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bmmidi/channel.hpp"

namespace {

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;

TEST(Status, ShouldCreateChannelVoice) {
  {
    const auto noteOffCh3 =
        bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOff, bmmidi::Channel::index(2));
    EXPECT_THAT(noteOffCh3.type(), Eq(bmmidi::MsgType::kNoteOff));
    EXPECT_THAT(noteOffCh3.value(), Eq(0x82));  // 0x8n, where n is 2 for channel index.
    EXPECT_THAT(noteOffCh3.numDataBytes(), Eq(2));
    EXPECT_THAT(noteOffCh3.isChannelSpecific(), IsTrue());
    EXPECT_THAT(noteOffCh3.channel().displayNumber(), Eq(3));
  }

  {
    const auto noteOnCh1 =
        bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn, bmmidi::Channel::first());
    EXPECT_THAT(noteOnCh1.type(), Eq(bmmidi::MsgType::kNoteOn));
    EXPECT_THAT(noteOnCh1.value(), Eq(0x90));  // 0x9n, where n is 0 for channel index.
    EXPECT_THAT(noteOnCh1.numDataBytes(), Eq(2));
    EXPECT_THAT(noteOnCh1.isChannelSpecific(), IsTrue());
    EXPECT_THAT(noteOnCh1.channel().displayNumber(), Eq(1));
  }

  {
    const auto keyAftertouchCh16 =
        bmmidi::Status::channelVoice(bmmidi::MsgType::kPolyphonicKeyPressure, bmmidi::Channel::last());
    EXPECT_THAT(keyAftertouchCh16.type(), Eq(bmmidi::MsgType::kPolyphonicKeyPressure));
    EXPECT_THAT(keyAftertouchCh16.value(), Eq(0xAF));  // 0xAn, where n is F for channel index.
    EXPECT_THAT(keyAftertouchCh16.numDataBytes(), Eq(2));
    EXPECT_THAT(keyAftertouchCh16.isChannelSpecific(), IsTrue());
    EXPECT_THAT(keyAftertouchCh16.channel().displayNumber(), Eq(16));
  }

  {
    const auto controlChangeCh10 =
        bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange, bmmidi::Channel::index(9));
    EXPECT_THAT(controlChangeCh10.type(), Eq(bmmidi::MsgType::kControlChange));
    EXPECT_THAT(controlChangeCh10.value(), Eq(0xB9));  // 0xBn, where n is 9 for channel index.
    EXPECT_THAT(controlChangeCh10.numDataBytes(), Eq(2));
    EXPECT_THAT(controlChangeCh10.isChannelSpecific(), IsTrue());
    EXPECT_THAT(controlChangeCh10.channel().displayNumber(), Eq(10));
  }

  {
    const auto programChangeCh13 =
        bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange, bmmidi::Channel::index(12));
    EXPECT_THAT(programChangeCh13.type(), Eq(bmmidi::MsgType::kProgramChange));
    EXPECT_THAT(programChangeCh13.value(), Eq(0xCC));  // 0xCn, where n is C for channel index.
    EXPECT_THAT(programChangeCh13.numDataBytes(), Eq(1));
    EXPECT_THAT(programChangeCh13.isChannelSpecific(), IsTrue());
    EXPECT_THAT(programChangeCh13.channel().displayNumber(), Eq(13));
  }

  {
    const auto channelAftertouchCh2 =
        bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure, bmmidi::Channel::index(1));
    EXPECT_THAT(channelAftertouchCh2.type(), Eq(bmmidi::MsgType::kChannelPressure));
    EXPECT_THAT(channelAftertouchCh2.value(), Eq(0xD1));  // 0xDn, where n is 1 for channel index.
    EXPECT_THAT(channelAftertouchCh2.numDataBytes(), Eq(1));
    EXPECT_THAT(channelAftertouchCh2.isChannelSpecific(), IsTrue());
    EXPECT_THAT(channelAftertouchCh2.channel().displayNumber(), Eq(2));
  }

  {
    const auto pitchBendCh7 =
        bmmidi::Status::channelVoice(bmmidi::MsgType::kPitchBend, bmmidi::Channel::index(6));
    EXPECT_THAT(pitchBendCh7.type(), Eq(bmmidi::MsgType::kPitchBend));
    EXPECT_THAT(pitchBendCh7.value(), Eq(0xE6));  // 0xEn, where n is 6 for channel index.
    EXPECT_THAT(pitchBendCh7.numDataBytes(), Eq(2));
    EXPECT_THAT(pitchBendCh7.isChannelSpecific(), IsTrue());
    EXPECT_THAT(pitchBendCh7.channel().displayNumber(), Eq(7));
  }
}

TEST(Status, ShouldCreateSystem) {
  // System Exclusive:

  {
    const auto sysEx = bmmidi::Status::system(bmmidi::MsgType::kSystemExclusive);
    EXPECT_THAT(sysEx.type(), Eq(bmmidi::MsgType::kSystemExclusive));
    EXPECT_THAT(sysEx.value(), Eq(0xF0));
    EXPECT_THAT(sysEx.numDataBytes(), Eq(bmmidi::Status::kVariableDataBytes));
    EXPECT_THAT(sysEx.isChannelSpecific(), IsFalse());
  }

  // System Common:

  {
    const auto mtcQuarterFrame = bmmidi::Status::system(bmmidi::MsgType::kMtcQuarterFrame);
    EXPECT_THAT(mtcQuarterFrame.type(), Eq(bmmidi::MsgType::kMtcQuarterFrame));
    EXPECT_THAT(mtcQuarterFrame.value(), Eq(0xF1));
    EXPECT_THAT(mtcQuarterFrame.numDataBytes(), Eq(1));
    EXPECT_THAT(mtcQuarterFrame.isChannelSpecific(), IsFalse());
  }

  {
    const auto songPos = bmmidi::Status::system(bmmidi::MsgType::kSongPositionPointer);
    EXPECT_THAT(songPos.type(), Eq(bmmidi::MsgType::kSongPositionPointer));
    EXPECT_THAT(songPos.value(), Eq(0xF2));
    EXPECT_THAT(songPos.numDataBytes(), Eq(2));
    EXPECT_THAT(songPos.isChannelSpecific(), IsFalse());
  }

  {
    const auto songSelect = bmmidi::Status::system(bmmidi::MsgType::kSongSelect);
    EXPECT_THAT(songSelect.type(), Eq(bmmidi::MsgType::kSongSelect));
    EXPECT_THAT(songSelect.value(), Eq(0xF3));
    EXPECT_THAT(songSelect.numDataBytes(), Eq(1));
    EXPECT_THAT(songSelect.isChannelSpecific(), IsFalse());
  }

  {
    const auto oscTune = bmmidi::Status::system(bmmidi::MsgType::kOscillatorTuneRequest);
    EXPECT_THAT(oscTune.type(), Eq(bmmidi::MsgType::kOscillatorTuneRequest));
    EXPECT_THAT(oscTune.value(), Eq(0xF6));
    EXPECT_THAT(oscTune.numDataBytes(), Eq(0));
    EXPECT_THAT(oscTune.isChannelSpecific(), IsFalse());
  }

  {
    const auto eox = bmmidi::Status::system(bmmidi::MsgType::kEndOfSystemExclusive);
    EXPECT_THAT(eox.type(), Eq(bmmidi::MsgType::kEndOfSystemExclusive));
    EXPECT_THAT(eox.value(), Eq(0xF7));
    EXPECT_THAT(eox.numDataBytes(), Eq(0));
    EXPECT_THAT(eox.isChannelSpecific(), IsFalse());
  }

  // System Real Time:

  {
    const auto clock = bmmidi::Status::system(bmmidi::MsgType::kTimingClock);
    EXPECT_THAT(clock.type(), Eq(bmmidi::MsgType::kTimingClock));
    EXPECT_THAT(clock.value(), Eq(0xF8));
    EXPECT_THAT(clock.numDataBytes(), Eq(0));
    EXPECT_THAT(clock.isChannelSpecific(), IsFalse());
  }

  {
    const auto start = bmmidi::Status::system(bmmidi::MsgType::kStart);
    EXPECT_THAT(start.type(), Eq(bmmidi::MsgType::kStart));
    EXPECT_THAT(start.value(), Eq(0xFA));
    EXPECT_THAT(start.numDataBytes(), Eq(0));
    EXPECT_THAT(start.isChannelSpecific(), IsFalse());
  }

  {
    const auto cont = bmmidi::Status::system(bmmidi::MsgType::kContinue);
    EXPECT_THAT(cont.type(), Eq(bmmidi::MsgType::kContinue));
    EXPECT_THAT(cont.value(), Eq(0xFB));
    EXPECT_THAT(cont.numDataBytes(), Eq(0));
    EXPECT_THAT(cont.isChannelSpecific(), IsFalse());
  }

  {
    const auto stop = bmmidi::Status::system(bmmidi::MsgType::kStop);
    EXPECT_THAT(stop.type(), Eq(bmmidi::MsgType::kStop));
    EXPECT_THAT(stop.value(), Eq(0xFC));
    EXPECT_THAT(stop.numDataBytes(), Eq(0));
    EXPECT_THAT(stop.isChannelSpecific(), IsFalse());
  }

  {
    const auto activeSense = bmmidi::Status::system(bmmidi::MsgType::kActiveSensing);
    EXPECT_THAT(activeSense.type(), Eq(bmmidi::MsgType::kActiveSensing));
    EXPECT_THAT(activeSense.value(), Eq(0xFE));
    EXPECT_THAT(activeSense.numDataBytes(), Eq(0));
    EXPECT_THAT(activeSense.isChannelSpecific(), IsFalse());
  }

  {
    const auto systemReset = bmmidi::Status::system(bmmidi::MsgType::kSystemReset);
    EXPECT_THAT(systemReset.type(), Eq(bmmidi::MsgType::kSystemReset));
    EXPECT_THAT(systemReset.value(), Eq(0xFF));
    EXPECT_THAT(systemReset.numDataBytes(), Eq(0));
    EXPECT_THAT(systemReset.isChannelSpecific(), IsFalse());
  }
}

TEST(Status, ShouldCreateFromByte) {
  {
    // 0x8n, where n is 2 for channel index.
    const auto noteOffCh3 = bmmidi::Status{0x82};
    EXPECT_THAT(noteOffCh3.type(), Eq(bmmidi::MsgType::kNoteOff));
    EXPECT_THAT(noteOffCh3.value(), Eq(0x82));
    EXPECT_THAT(noteOffCh3.numDataBytes(), Eq(2));
    EXPECT_THAT(noteOffCh3.isChannelSpecific(), IsTrue());
    EXPECT_THAT(noteOffCh3.channel().displayNumber(), Eq(3));
  }

  {
    const auto songPos = bmmidi::Status{0xF2};
    EXPECT_THAT(songPos.type(), Eq(bmmidi::MsgType::kSongPositionPointer));
    EXPECT_THAT(songPos.value(), Eq(0xF2));
    EXPECT_THAT(songPos.numDataBytes(), Eq(2));
    EXPECT_THAT(songPos.isChannelSpecific(), IsFalse());
  }
}

TEST(Status, ShouldSupportEqualityComparison) {
  const auto noteOffCh3 =
      bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOff, bmmidi::Channel::index(2));
  const auto noteOffCh4 =
      bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOff, bmmidi::Channel::index(3));
  const auto alsoNoteOffCh4 = bmmidi::Status{0x83};

  EXPECT_THAT(noteOffCh3 == noteOffCh4, IsFalse());
  EXPECT_THAT(noteOffCh4 == noteOffCh3, IsFalse());
  EXPECT_THAT(noteOffCh3 != noteOffCh4, IsTrue());
  EXPECT_THAT(noteOffCh4 != noteOffCh3, IsTrue());

  EXPECT_THAT(noteOffCh4 == noteOffCh4, IsTrue());
  EXPECT_THAT(noteOffCh4 != noteOffCh4, IsFalse());

  EXPECT_THAT(noteOffCh4 == alsoNoteOffCh4, IsTrue());
  EXPECT_THAT(alsoNoteOffCh4 == noteOffCh4, IsTrue());
  EXPECT_THAT(noteOffCh4 != alsoNoteOffCh4, IsFalse());
  EXPECT_THAT(alsoNoteOffCh4 != noteOffCh4, IsFalse());

  const auto sysEx = bmmidi::Status::system(bmmidi::MsgType::kSystemExclusive);
  const auto songPos = bmmidi::Status::system(bmmidi::MsgType::kSongPositionPointer);
  const auto alsoSongPos = bmmidi::Status{0xF2};

  EXPECT_THAT(noteOffCh3 == sysEx, IsFalse());
  EXPECT_THAT(sysEx == noteOffCh3, IsFalse());
  EXPECT_THAT(noteOffCh3 != sysEx, IsTrue());
  EXPECT_THAT(sysEx != noteOffCh3, IsTrue());

  EXPECT_THAT(sysEx == songPos, IsFalse());
  EXPECT_THAT(songPos == sysEx, IsFalse());
  EXPECT_THAT(sysEx != songPos, IsTrue());
  EXPECT_THAT(songPos != sysEx, IsTrue());

  EXPECT_THAT(songPos == songPos, IsTrue());
  EXPECT_THAT(songPos != songPos, IsFalse());

  EXPECT_THAT(songPos == alsoSongPos, IsTrue());
  EXPECT_THAT(alsoSongPos == songPos, IsTrue());
  EXPECT_THAT(songPos != alsoSongPos, IsFalse());
  EXPECT_THAT(alsoSongPos != songPos, IsFalse());
}

}  // namespace
