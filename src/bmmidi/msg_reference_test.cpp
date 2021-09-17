#include "bmmidi/msg_reference.hpp"
#include "bmmidi/msg.hpp"

#include <cstdint>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bmmidi/channel.hpp"
#include "bmmidi/control.hpp"
#include "bmmidi/data_value.hpp"
#include "bmmidi/status.hpp"
#include "bmmidi/sysex.hpp"

namespace {

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;

TEST(MsgView, WorksForStandaloneBytes) {
  // Control Change, channel 13, CC #1 (Mod Wheel), value 37.
  const std::uint8_t srcBytes[] = {0xBC, 0x01, 0x25};
  bmmidi::MsgView view{srcBytes, sizeof(srcBytes)};

  // Read-only accessors:
  EXPECT_THAT(view.type(), Eq(bmmidi::MsgType::kControlChange));
  EXPECT_THAT(view.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(12))));
  EXPECT_THAT(view.data1(), Eq(bmmidi::DataValue{1}));
  EXPECT_THAT(view.data2(), Eq(bmmidi::DataValue{37}));
  EXPECT_THAT(view.numBytes(), Eq(3));

  EXPECT_THAT(view.rawBytes()[0], Eq(0xBC));
  EXPECT_THAT(view.rawBytes()[1], Eq(0x01));
  EXPECT_THAT(view.rawBytes()[2], Eq(0x25));
}

TEST(MsgView, WorksForMsg) {
  // Control Change, channel 13, CC #1 (Mod Wheel), value 37.
  bmmidi::Msg<3> ccCh13Mod37{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(12)),
      bmmidi::controlToDataValue(bmmidi::Control::kModWheel),
      bmmidi::DataValue{37}};
  auto view = ccCh13Mod37.asView<bmmidi::MsgView>();

  // Read-only accessors:
  EXPECT_THAT(view.type(), Eq(bmmidi::MsgType::kControlChange));
  EXPECT_THAT(view.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(12))));
  EXPECT_THAT(view.data1(), Eq(bmmidi::DataValue{1}));
  EXPECT_THAT(view.data2(), Eq(bmmidi::DataValue{37}));
  EXPECT_THAT(view.numBytes(), Eq(3));

  EXPECT_THAT(view.rawBytes()[0], Eq(0xBC));
  EXPECT_THAT(view.rawBytes()[1], Eq(0x01));
  EXPECT_THAT(view.rawBytes()[2], Eq(0x25));
}

TEST(MsgView, ComparesByteValues) {
  // Should be able to compare a Msg to raw bytes through MsgView interface.
  // Control Change, channel 13, CC #1 (Mod Wheel), value 37.
  const bmmidi::Msg<3> msg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(12)),
      bmmidi::controlToDataValue(bmmidi::Control::kModWheel),
      bmmidi::DataValue{37}};
  const auto msgView = msg.asView<bmmidi::MsgView>();

  // (Same, just as raw bytes).
  const std::uint8_t rawBytes[] = {0xBC, 0x01, 0x25};
  const bmmidi::MsgView rawBytesView{rawBytes, sizeof(rawBytes)};

  EXPECT_THAT(msgView.hasSameValueAs(rawBytesView), IsTrue());
  EXPECT_THAT(rawBytesView.hasSameValueAs(msgView), IsTrue());

  // A different Msg<3> value should compare as unequal.
  const bmmidi::Msg<3> differentMsg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(12)),
      bmmidi::controlToDataValue(bmmidi::Control::kModWheel),
      bmmidi::DataValue{38}};  // Value 38 instead of 37.
  const auto differentMsgView = differentMsg.asView<bmmidi::MsgView>();

  EXPECT_THAT(msgView.hasSameValueAs(differentMsgView), IsFalse());
  EXPECT_THAT(differentMsgView.hasSameValueAs(msgView), IsFalse());

  EXPECT_THAT(rawBytesView.hasSameValueAs(differentMsgView), IsFalse());
  EXPECT_THAT(differentMsgView.hasSameValueAs(rawBytesView), IsFalse());
}

TEST(MsgRef, WorksForStandaloneBytes) {
  // Control Change, channel 13, CC #1 (Mod Wheel), value 37.
  std::uint8_t srcBytes[] = {0xBC, 0x01, 0x25};
  bmmidi::MsgRef ref{srcBytes, sizeof(srcBytes)};

  // Read-only accessors are still available:
  EXPECT_THAT(ref.type(), Eq(bmmidi::MsgType::kControlChange));
  EXPECT_THAT(ref.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(12))));
  EXPECT_THAT(ref.data1(), Eq(bmmidi::DataValue{1}));
  EXPECT_THAT(ref.data2(), Eq(bmmidi::DataValue{37}));
  EXPECT_THAT(ref.numBytes(), Eq(3));

  EXPECT_THAT(ref.rawBytes()[0], Eq(0xBC));
  EXPECT_THAT(ref.rawBytes()[1], Eq(0x01));
  EXPECT_THAT(ref.rawBytes()[2], Eq(0x25));

  // Mutations:
  ref.setData2(bmmidi::DataValue{126});
  EXPECT_THAT(ref.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(12))));
  EXPECT_THAT(ref.data1(), Eq(bmmidi::DataValue{1}));
  EXPECT_THAT(ref.data2(), Eq(bmmidi::DataValue{126}));

  ref.setData1(bmmidi::DataValue{0x45});
  EXPECT_THAT(ref.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(12))));
  EXPECT_THAT(ref.data1(), Eq(bmmidi::DataValue{69}));
  EXPECT_THAT(ref.data2(), Eq(bmmidi::DataValue{126}));

  ref.setStatus(
      bmmidi::Status::channelVoice(bmmidi::MsgType::kPolyphonicKeyPressure,
                                   bmmidi::Channel::index(15)));
  EXPECT_THAT(ref.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kPolyphonicKeyPressure,
                                      bmmidi::Channel::index(15))));
  EXPECT_THAT(ref.data1(), Eq(bmmidi::DataValue{69}));
  EXPECT_THAT(ref.data2(), Eq(bmmidi::DataValue{126}));

  // This should affect srcBytes[] data too:
  EXPECT_THAT(srcBytes[0], Eq(0xAF));
  EXPECT_THAT(srcBytes[1], Eq(0x45));
  EXPECT_THAT(srcBytes[2], Eq(0x7E));

  // Can also mutate through rawBytes():
  ref.rawBytes()[0] = 0x92;
  ref.rawBytes()[1] = 0x44;
  ref.rawBytes()[2] = 0x5A;

  EXPECT_THAT(ref.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(ref.data1(), Eq(bmmidi::DataValue{68}));
  EXPECT_THAT(ref.data2(), Eq(bmmidi::DataValue{90}));

  EXPECT_THAT(srcBytes[0], Eq(0x92));
  EXPECT_THAT(srcBytes[1], Eq(0x44));
  EXPECT_THAT(srcBytes[2], Eq(0x5A));
}

TEST(MsgRef, WorksForMsg) {
  // Control Change, channel 13, CC #1 (Mod Wheel), value 37.
  bmmidi::Msg<3> ccCh13Mod37{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(12)),
      bmmidi::controlToDataValue(bmmidi::Control::kModWheel),
      bmmidi::DataValue{37}};
  auto ref = ccCh13Mod37.asRef<bmmidi::MsgRef>();

  // Read-only accessors are still available:
  EXPECT_THAT(ref.type(), Eq(bmmidi::MsgType::kControlChange));
  EXPECT_THAT(ref.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(12))));
  EXPECT_THAT(ref.data1(), Eq(bmmidi::DataValue{1}));
  EXPECT_THAT(ref.data2(), Eq(bmmidi::DataValue{37}));
  EXPECT_THAT(ref.numBytes(), Eq(3));

  EXPECT_THAT(ref.rawBytes()[0], Eq(0xBC));
  EXPECT_THAT(ref.rawBytes()[1], Eq(0x01));
  EXPECT_THAT(ref.rawBytes()[2], Eq(0x25));

  // Mutations:
  ref.setData2(bmmidi::DataValue{126});
  EXPECT_THAT(ref.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(12))));
  EXPECT_THAT(ref.data1(), Eq(bmmidi::DataValue{1}));
  EXPECT_THAT(ref.data2(), Eq(bmmidi::DataValue{126}));

  ref.setData1(bmmidi::DataValue{0x45});
  EXPECT_THAT(ref.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(12))));
  EXPECT_THAT(ref.data1(), Eq(bmmidi::DataValue{69}));
  EXPECT_THAT(ref.data2(), Eq(bmmidi::DataValue{126}));

  ref.setStatus(
      bmmidi::Status::channelVoice(bmmidi::MsgType::kPolyphonicKeyPressure,
                                   bmmidi::Channel::index(15)));
  EXPECT_THAT(ref.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kPolyphonicKeyPressure,
                                      bmmidi::Channel::index(15))));
  EXPECT_THAT(ref.data1(), Eq(bmmidi::DataValue{69}));
  EXPECT_THAT(ref.data2(), Eq(bmmidi::DataValue{126}));

  // This should affect ccCh13Mod37 data too:
  EXPECT_THAT(ccCh13Mod37.rawBytes()[0], Eq(0xAF));
  EXPECT_THAT(ccCh13Mod37.rawBytes()[1], Eq(0x45));
  EXPECT_THAT(ccCh13Mod37.rawBytes()[2], Eq(0x7E));

  // Can also mutate through rawBytes():
  ref.rawBytes()[0] = 0x92;
  ref.rawBytes()[1] = 0x44;
  ref.rawBytes()[2] = 0x5A;

  EXPECT_THAT(ref.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(ref.data1(), Eq(bmmidi::DataValue{68}));
  EXPECT_THAT(ref.data2(), Eq(bmmidi::DataValue{90}));

  EXPECT_THAT(ccCh13Mod37.rawBytes()[0], Eq(0x92));
  EXPECT_THAT(ccCh13Mod37.rawBytes()[1], Eq(0x44));
  EXPECT_THAT(ccCh13Mod37.rawBytes()[2], Eq(0x5A));
}

// TODO: Add tests for MsgView and MsgRef referring to SysExMsg.
// TODO: Add at least one test with TimedMsgView, TimedMsgRef.

TEST(ChanMsgView, CanReferToRawBytes) {
  // (Note On, channel 3, key 69, velocity 90).
  const std::uint8_t srcBytes[] = {0x92, 0x45, 0x5A};
  bmmidi::ChanMsgView chanMsgView{srcBytes, sizeof(srcBytes)};

  // Can use channel accessor:
  EXPECT_THAT(chanMsgView.channel().displayNumber(), Eq(3));

  // Can also still use MsgView accessors:
  EXPECT_THAT(chanMsgView.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(chanMsgView.data1(), Eq(bmmidi::DataValue{69}));
  EXPECT_THAT(chanMsgView.data2(), Eq(bmmidi::DataValue{90}));
}

TEST(ChanMsgRef, CanReferToRawBytes) {
  // (Note On, channel 3, key 69, velocity 90).
  std::uint8_t srcBytes[] = {0x92, 0x45, 0x5A};
  bmmidi::ChanMsgRef chanMsgRef{srcBytes, sizeof(srcBytes)};

  // Can use channel accessor:
  EXPECT_THAT(chanMsgRef.channel().displayNumber(), Eq(3));

  // Can also still use MsgRef accessors:
  EXPECT_THAT(chanMsgRef.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(chanMsgRef.data1(), Eq(bmmidi::DataValue{69}));
  EXPECT_THAT(chanMsgRef.data2(), Eq(bmmidi::DataValue{90}));

  // Can mutate the referenced bytes:
  chanMsgRef.setChannel(bmmidi::Channel::index(13));
  EXPECT_THAT(chanMsgRef.channel().displayNumber(), Eq(14));
  EXPECT_THAT(srcBytes[0], Eq(0x9D));
}

TEST(NoteMsgView, CanReferToRawBytes) {
  // (Note On, channel 3, key 69, velocity 90).
  const std::uint8_t srcBytes[] = {0x92, 0x45, 0x5A};
  bmmidi::NoteMsgView noteMsgView{srcBytes, sizeof(srcBytes)};

  // Can use NoteMsgView accessors:
  EXPECT_THAT(noteMsgView.isNoteOn(), IsTrue());
  EXPECT_THAT(noteMsgView.isNoteOff(), IsFalse());
  EXPECT_THAT(noteMsgView.key().value(), Eq(69));
  EXPECT_THAT(noteMsgView.velocity().value(), Eq(90));

  // Can also still use base accessors:
  EXPECT_THAT(noteMsgView.channel().displayNumber(), Eq(3));
  EXPECT_THAT(noteMsgView.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(noteMsgView.data1(), Eq(bmmidi::DataValue{69}));
  EXPECT_THAT(noteMsgView.data2(), Eq(bmmidi::DataValue{90}));
}

TEST(NoteMsgView, TreatsNoteOnZeroVelocityAsNoteOff) {
  // (Note "On", channel 3, key 69, velocity 0).
  const std::uint8_t srcBytes[] = {0x92, 0x45, 0x00};
  bmmidi::NoteMsgView noteMsgView{srcBytes, sizeof(srcBytes)};

  // Can use NoteMsgView accessors:
  EXPECT_THAT(noteMsgView.isNoteOn(), IsFalse());
  EXPECT_THAT(noteMsgView.isNoteOff(), IsTrue());  // Treats as note off!
  EXPECT_THAT(noteMsgView.key().value(), Eq(69));
  EXPECT_THAT(noteMsgView.velocity().value(), Eq(0));

  // Can also still use base accessors:
  EXPECT_THAT(noteMsgView.channel().displayNumber(), Eq(3));
  EXPECT_THAT(noteMsgView.status(),  // But status is still kNoteOn.
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(noteMsgView.data1(), Eq(bmmidi::DataValue{69}));
  EXPECT_THAT(noteMsgView.data2(), Eq(bmmidi::DataValue{0}));
}

TEST(NoteMsgView, ShouldConvertFromMsgView) {
  // A more generic MsgView...
  // (Note On, channel 3, key 69, velocity 90).
  const std::uint8_t srcBytes[] = {0x92, 0x45, 0x5A};
  bmmidi::MsgView msgView{srcBytes, sizeof(srcBytes)};

  // ... should be convertible to the more specific NoteMsgView.
  const auto noteMsgView = msgView.asView<bmmidi::NoteMsgView>();

  EXPECT_THAT(noteMsgView.channel().displayNumber(), Eq(3));
  EXPECT_THAT(noteMsgView.isNoteOff(), IsFalse());
  EXPECT_THAT(noteMsgView.isNoteOn(), IsTrue());
  EXPECT_THAT(noteMsgView.key().value(), Eq(69));
  EXPECT_THAT(noteMsgView.velocity().value(), Eq(90));
}

TEST(NoteMsgView, ShouldConvertFromMsgRef) {
  // A more generic MsgRef (which is also read-write)...
  // (Note On, channel 3, key 69, velocity 90).
  std::uint8_t srcBytes[] = {0x92, 0x45, 0x5A};
  bmmidi::MsgRef msgRef{srcBytes, sizeof(srcBytes)};

  // ... should be convertible to the more specific NoteMsgView
  // (which is only read-only).
  const auto noteMsgView = msgRef.asView<bmmidi::NoteMsgView>();

  EXPECT_THAT(noteMsgView.channel().displayNumber(), Eq(3));
  EXPECT_THAT(noteMsgView.isNoteOff(), IsFalse());
  EXPECT_THAT(noteMsgView.isNoteOn(), IsTrue());
  EXPECT_THAT(noteMsgView.key().value(), Eq(69));
  EXPECT_THAT(noteMsgView.velocity().value(), Eq(90));
}

TEST(NoteMsgView, ShouldConvertFromMsg) {
  // A more generic Msg...
  // (Note On, channel 3, key 69, velocity 90).
  const bmmidi::Msg<3> msg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                   bmmidi::Channel::index(2)),
      bmmidi::DataValue{69},
      bmmidi::DataValue{90}};
  
  // ...should be able to be referenced with a more specific NoteMsgView...
  auto noteOnView = msg.asView<bmmidi::NoteMsgView>();

  EXPECT_THAT(noteOnView.channel().displayNumber(), Eq(3));
  EXPECT_THAT(noteOnView.isNoteOff(), IsFalse());
  EXPECT_THAT(noteOnView.isNoteOn(), IsTrue());
  EXPECT_THAT(noteOnView.key().value(), Eq(69));
  EXPECT_THAT(noteOnView.velocity().value(), Eq(90));
}

TEST(NoteMsgRef, CanReferToRawBytes) {
  // (Note Off, channel 3, key 69, velocity 90).
  std::uint8_t srcBytes[] = {0x82, 0x45, 0x5A};
  bmmidi::NoteMsgRef noteMsgRef{srcBytes, sizeof(srcBytes)};

  // Can use NoteMsgRef accessors:
  EXPECT_THAT(noteMsgRef.isNoteOn(), IsFalse());
  EXPECT_THAT(noteMsgRef.isNoteOff(), IsTrue());
  EXPECT_THAT(noteMsgRef.key().value(), Eq(69));
  EXPECT_THAT(noteMsgRef.velocity().value(), Eq(90));

  // Can also still use base accessors:
  EXPECT_THAT(noteMsgRef.channel().displayNumber(), Eq(3));
  EXPECT_THAT(noteMsgRef.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOff,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(noteMsgRef.data1(), Eq(bmmidi::DataValue{69}));
  EXPECT_THAT(noteMsgRef.data2(), Eq(bmmidi::DataValue{90}));

  // Can mutate:
  noteMsgRef.setType(bmmidi::MsgType::kNoteOn);
  noteMsgRef.setChannel(bmmidi::Channel::index(13));
  noteMsgRef.setKey(bmmidi::KeyNumber::key(60));
  noteMsgRef.setVelocity(bmmidi::DataValue{22});

  EXPECT_THAT(noteMsgRef.isNoteOn(), IsTrue());
  EXPECT_THAT(noteMsgRef.isNoteOff(), IsFalse());
  EXPECT_THAT(noteMsgRef.key().value(), Eq(60));
  EXPECT_THAT(noteMsgRef.velocity().value(), Eq(22));
  EXPECT_THAT(noteMsgRef.channel().displayNumber(), Eq(14));
  EXPECT_THAT(noteMsgRef.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                      bmmidi::Channel::index(13))));
  EXPECT_THAT(noteMsgRef.data1(), Eq(bmmidi::DataValue{60}));
  EXPECT_THAT(noteMsgRef.data2(), Eq(bmmidi::DataValue{22}));

  EXPECT_THAT(srcBytes[0], Eq(0x9D));
  EXPECT_THAT(srcBytes[1], Eq(0x3C));
  EXPECT_THAT(srcBytes[2], Eq(0x16));
}

TEST(NoteMsgRef, ShouldConvertFromMsgRef) {
  // A more generic MsgRef (which is read-write)...
  // (Note On, channel 3, key 69, velocity 90).
  std::uint8_t srcBytes[] = {0x92, 0x45, 0x5A};
  bmmidi::MsgRef msgRef{srcBytes, sizeof(srcBytes)};

  // ...should be convertible to the more specific NoteMsgRef...
  auto noteMsgRef = msgRef.asRef<bmmidi::NoteMsgRef>();

  EXPECT_THAT(noteMsgRef.channel().displayNumber(), Eq(3));
  EXPECT_THAT(noteMsgRef.isNoteOff(), IsFalse());
  EXPECT_THAT(noteMsgRef.isNoteOn(), IsTrue());
  EXPECT_THAT(noteMsgRef.key().value(), Eq(69));
  EXPECT_THAT(noteMsgRef.velocity().value(), Eq(90));

  // ...which is also read-write.
  noteMsgRef.setChannel(bmmidi::Channel::index(6));
  EXPECT_THAT(noteMsgRef.channel().displayNumber(), Eq(7));
  EXPECT_THAT(srcBytes[0], Eq(0x96));
}

TEST(NoteMsgRef, ShouldConvertFromMsg) {
  // A more generic Msg (which is read-write)...
  // (Note On, channel 3, key 69, velocity 90).
  bmmidi::Msg<3> msg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                   bmmidi::Channel::index(2)),
      bmmidi::DataValue{69},
      bmmidi::DataValue{90}};
  
  // ...should be able to be referenced with a more specific NoteMsgRef...
  auto noteOnRef = msg.asRef<bmmidi::NoteMsgRef>();

  EXPECT_THAT(noteOnRef.channel().displayNumber(), Eq(3));
  EXPECT_THAT(noteOnRef.isNoteOff(), IsFalse());
  EXPECT_THAT(noteOnRef.isNoteOn(), IsTrue());
  EXPECT_THAT(noteOnRef.key().value(), Eq(69));
  EXPECT_THAT(noteOnRef.velocity().value(), Eq(90));

  // ...which is also read-write.
  noteOnRef.setVelocity(bmmidi::DataValue{23});
  EXPECT_THAT(noteOnRef.velocity().value(), Eq(23));
  EXPECT_THAT(msg.data2().value(), Eq(23));
}

TEST(KeyPressureMsgView, CanReferToRawBytes) {
  // (Polyphonic Key Pressure, channel 3, key 69, pressure 90).
  const std::uint8_t srcBytes[] = {0xA2, 0x45, 0x5A};
  bmmidi::KeyPressureMsgView kpMsgView{srcBytes, sizeof(srcBytes)};

  // Can use KeyPressureMsgView accessors:
  EXPECT_THAT(kpMsgView.key().value(), Eq(69));
  EXPECT_THAT(kpMsgView.pressure().value(), Eq(90));

  // Can also still use base accessors:
  EXPECT_THAT(kpMsgView.channel().displayNumber(), Eq(3));
  EXPECT_THAT(kpMsgView.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kPolyphonicKeyPressure,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(kpMsgView.data1(), Eq(bmmidi::DataValue{69}));
  EXPECT_THAT(kpMsgView.data2(), Eq(bmmidi::DataValue{90}));
}

TEST(KeyPressureMsgRef, CanReferToRawBytes) {
  // (Polyphonic Key Pressure, channel 3, key 69, pressure 90).
  std::uint8_t srcBytes[] = {0xA2, 0x45, 0x5A};
  bmmidi::KeyPressureMsgRef kpMsgRef{srcBytes, sizeof(srcBytes)};

  // Can use KeyPressureMsgRef accessors:
  EXPECT_THAT(kpMsgRef.key().value(), Eq(69));
  EXPECT_THAT(kpMsgRef.pressure().value(), Eq(90));

  // Can also still use base accessors:
  EXPECT_THAT(kpMsgRef.channel().displayNumber(), Eq(3));
  EXPECT_THAT(kpMsgRef.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kPolyphonicKeyPressure,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(kpMsgRef.data1(), Eq(bmmidi::DataValue{69}));
  EXPECT_THAT(kpMsgRef.data2(), Eq(bmmidi::DataValue{90}));

  // Can mutate:
  kpMsgRef.setChannel(bmmidi::Channel::index(13));
  kpMsgRef.setKey(bmmidi::KeyNumber::key(60));
  kpMsgRef.setPressure(bmmidi::DataValue{22});

  EXPECT_THAT(kpMsgRef.key().value(), Eq(60));
  EXPECT_THAT(kpMsgRef.pressure().value(), Eq(22));
  EXPECT_THAT(kpMsgRef.channel().displayNumber(), Eq(14));
  EXPECT_THAT(kpMsgRef.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kPolyphonicKeyPressure,
                                      bmmidi::Channel::index(13))));
  EXPECT_THAT(kpMsgRef.data1(), Eq(bmmidi::DataValue{60}));
  EXPECT_THAT(kpMsgRef.data2(), Eq(bmmidi::DataValue{22}));

  EXPECT_THAT(srcBytes[0], Eq(0xAD));
  EXPECT_THAT(srcBytes[1], Eq(0x3C));
  EXPECT_THAT(srcBytes[2], Eq(0x16));
}

TEST(ControlChangeMsgView, CanReferToRawBytes) {
  // (Control Change, channel 3, CC #11 expression, value 90).
  const std::uint8_t srcBytes[] = {0xB2, 0x0B, 0x5A};
  bmmidi::ControlChangeMsgView ccMsgView{srcBytes, sizeof(srcBytes)};

  // Can use ControlChangeMsgView accessors:
  EXPECT_THAT(ccMsgView.control(), Eq(bmmidi::Control::kExpression));
  EXPECT_THAT(ccMsgView.value(), Eq(bmmidi::DataValue{90}));

  // Can also still use base accessors:
  EXPECT_THAT(ccMsgView.channel().displayNumber(), Eq(3));
  EXPECT_THAT(ccMsgView.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(ccMsgView.data1(), Eq(bmmidi::DataValue{11}));
  EXPECT_THAT(ccMsgView.data2(), Eq(bmmidi::DataValue{90}));
}

TEST(ControlChangeMsgRef, CanReferToRawBytes) {
  // (Control Change, channel 3, CC #11 expression, value 90).
  std::uint8_t srcBytes[] = {0xB2, 0x0B, 0x5A};
  bmmidi::ControlChangeMsgRef ccMsgRef{srcBytes, sizeof(srcBytes)};

  // Can use ControlChangeMsgRef accessors:
  EXPECT_THAT(ccMsgRef.control(), Eq(bmmidi::Control::kExpression));
  EXPECT_THAT(ccMsgRef.value(), Eq(bmmidi::DataValue{90}));

  // Can also still use base accessors:
  EXPECT_THAT(ccMsgRef.channel().displayNumber(), Eq(3));
  EXPECT_THAT(ccMsgRef.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(ccMsgRef.data1(), Eq(bmmidi::DataValue{11}));
  EXPECT_THAT(ccMsgRef.data2(), Eq(bmmidi::DataValue{90}));

  // Can mutate:
  ccMsgRef.setChannel(bmmidi::Channel::index(13));
  ccMsgRef.setControl(bmmidi::Control::kModWheel);
  ccMsgRef.setValue(bmmidi::DataValue{22});

  EXPECT_THAT(ccMsgRef.control(), Eq(bmmidi::Control::kModWheel));
  EXPECT_THAT(ccMsgRef.value(), Eq(bmmidi::DataValue{22}));
  EXPECT_THAT(ccMsgRef.channel().displayNumber(), Eq(14));
  EXPECT_THAT(ccMsgRef.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(13))));
  EXPECT_THAT(ccMsgRef.data1(), Eq(bmmidi::DataValue{1}));
  EXPECT_THAT(ccMsgRef.data2(), Eq(bmmidi::DataValue{22}));

  EXPECT_THAT(srcBytes[0], Eq(0xBD));
  EXPECT_THAT(srcBytes[1], Eq(0x01));
  EXPECT_THAT(srcBytes[2], Eq(0x16));
}

TEST(ProgramChangeMsgView, CanReferToRawBytes) {
  // (Program Change, channel 3, program 57).
  const std::uint8_t srcBytes[] = {0xC2, 0x38};
  bmmidi::ProgramChangeMsgView pcMsgView{srcBytes, sizeof(srcBytes)};

  // Can use ProgramChangeMsgView accessors:
  EXPECT_THAT(pcMsgView.program().displayNumber(), Eq(57));

  // Can also still use base accessors:
  EXPECT_THAT(pcMsgView.channel().displayNumber(), Eq(3));
  EXPECT_THAT(pcMsgView.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(pcMsgView.data1(), Eq(bmmidi::DataValue{56}));
}

TEST(ProgramChangeMsgRef, CanReferToRawBytes) {
  // (Program Change, channel 3, program 57).
  std::uint8_t srcBytes[] = {0xC2, 0x38};
  bmmidi::ProgramChangeMsgRef pcMsgRef{srcBytes, sizeof(srcBytes)};

  // Can use ProgramChangeMsgRef accessors:
  EXPECT_THAT(pcMsgRef.program().displayNumber(), Eq(57));

  // Can also still use base accessors:
  EXPECT_THAT(pcMsgRef.channel().displayNumber(), Eq(3));
  EXPECT_THAT(pcMsgRef.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(pcMsgRef.data1(), Eq(bmmidi::DataValue{56}));

  // Can mutate:
  pcMsgRef.setChannel(bmmidi::Channel::index(13));
  pcMsgRef.setProgram(bmmidi::PresetNumber::index(22));

  EXPECT_THAT(pcMsgRef.program().displayNumber(), Eq(23));
  EXPECT_THAT(pcMsgRef.channel().displayNumber(), Eq(14));
  EXPECT_THAT(pcMsgRef.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                      bmmidi::Channel::index(13))));
  EXPECT_THAT(pcMsgRef.data1(), Eq(bmmidi::DataValue{22}));

  EXPECT_THAT(srcBytes[0], Eq(0xCD));
  EXPECT_THAT(srcBytes[1], Eq(0x16));
}

TEST(ChanPressureMsgView, CanReferToRawBytes) {
  // (Channel Pressure, channel 3, pressure 56).
  const std::uint8_t srcBytes[] = {0xD2, 0x38};
  bmmidi::ChanPressureMsgView cpMsgView{srcBytes, sizeof(srcBytes)};

  // Can use ChanPressureMsgView accessors:
  EXPECT_THAT(cpMsgView.pressure().value(), Eq(56));

  // Can also still use base accessors:
  EXPECT_THAT(cpMsgView.channel().displayNumber(), Eq(3));
  EXPECT_THAT(cpMsgView.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(cpMsgView.data1(), Eq(bmmidi::DataValue{56}));
}

TEST(ChanPressureMsgRef, CanReferToRawBytes) {
  // (Channel Pressure, channel 3, pressure 56).
  std::uint8_t srcBytes[] = {0xD2, 0x38};
  bmmidi::ChanPressureMsgRef cpMsgRef{srcBytes, sizeof(srcBytes)};

  // Can use ChanPressureMsgRef accessors:
  EXPECT_THAT(cpMsgRef.pressure().value(), Eq(56));

  // Can also still use base accessors:
  EXPECT_THAT(cpMsgRef.channel().displayNumber(), Eq(3));
  EXPECT_THAT(cpMsgRef.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(cpMsgRef.data1(), Eq(bmmidi::DataValue{56}));

  // Can mutate:
  cpMsgRef.setChannel(bmmidi::Channel::index(13));
  cpMsgRef.setPressure(bmmidi::DataValue{22});

  EXPECT_THAT(cpMsgRef.pressure().value(), Eq(22));
  EXPECT_THAT(cpMsgRef.channel().displayNumber(), Eq(14));
  EXPECT_THAT(cpMsgRef.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                      bmmidi::Channel::index(13))));
  EXPECT_THAT(cpMsgRef.data1(), Eq(bmmidi::DataValue{22}));

  EXPECT_THAT(srcBytes[0], Eq(0xDD));
  EXPECT_THAT(srcBytes[1], Eq(0x16));
}

TEST(PitchBendMsgView, CanReferToRawBytes) {
  // (Pitch Bend, channel 3, bend 15243).
  // LSB:            0b000 1011 (  0x0B = 11)
  // MSB:   0b11 1011 1         (  0x77 = 119)
  // Val: 0b  11 1011 1000 1011 (0x3B8B = 15243)
  const std::uint8_t srcBytes[] = {0xE2, 0x0B, 0x77};
  bmmidi::PitchBendMsgView pbMsgView{srcBytes, sizeof(srcBytes)};

  // Can use PitchBendMsgView accessors:
  EXPECT_THAT(pbMsgView.bend().value(), Eq(15243));
  EXPECT_THAT(pbMsgView.bend().lsb(), Eq(0x0B));
  EXPECT_THAT(pbMsgView.bend().msb(), Eq(0x77));

  // Can also still use base accessors:
  EXPECT_THAT(pbMsgView.channel().displayNumber(), Eq(3));
  EXPECT_THAT(pbMsgView.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kPitchBend,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(pbMsgView.data1(), Eq(bmmidi::DataValue{0x0B}));
  EXPECT_THAT(pbMsgView.data2(), Eq(bmmidi::DataValue{0x77}));
}

TEST(PitchBendMsgRef, CanReferToRawBytes) {
  // (Pitch Bend, channel 3, bend 15243).
  // LSB:            0b000 1011 (  0x0B = 11)
  // MSB:   0b11 1011 1         (  0x77 = 119)
  // Val: 0b  11 1011 1000 1011 (0x3B8B = 15243)
  std::uint8_t srcBytes[] = {0xE2, 0x0B, 0x77};
  bmmidi::PitchBendMsgRef pbMsgRef{srcBytes, sizeof(srcBytes)};

  // Can use PitchBendMsgRef accessors:
  EXPECT_THAT(pbMsgRef.bend().value(), Eq(15243));
  EXPECT_THAT(pbMsgRef.bend().lsb(), Eq(0x0B));
  EXPECT_THAT(pbMsgRef.bend().msb(), Eq(0x77));

  // Can also still use base accessors:
  EXPECT_THAT(pbMsgRef.channel().displayNumber(), Eq(3));
  EXPECT_THAT(pbMsgRef.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kPitchBend,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(pbMsgRef.data1(), Eq(bmmidi::DataValue{0x0B}));
  EXPECT_THAT(pbMsgRef.data2(), Eq(bmmidi::DataValue{0x77}));

  // Can mutate:
  pbMsgRef.setChannel(bmmidi::Channel::index(13));
  pbMsgRef.setBend(bmmidi::PitchBend::min());  // Uses 1 instead of 0, for symmetry.

  EXPECT_THAT(pbMsgRef.bend().value(), Eq(1));
  EXPECT_THAT(pbMsgRef.bend().lsb(), Eq(0x01));
  EXPECT_THAT(pbMsgRef.bend().msb(), Eq(0x00));
  EXPECT_THAT(pbMsgRef.channel().displayNumber(), Eq(14));
  EXPECT_THAT(pbMsgRef.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kPitchBend,
                                      bmmidi::Channel::index(13))));
  EXPECT_THAT(pbMsgRef.data1(), Eq(bmmidi::DataValue{0x01}));
  EXPECT_THAT(pbMsgRef.data2(), Eq(bmmidi::DataValue{0x00}));

  EXPECT_THAT(srcBytes[0], Eq(0xED));
  EXPECT_THAT(srcBytes[1], Eq(0x01));
  EXPECT_THAT(srcBytes[2], Eq(0x00));
}

TEST(SysExMsgView, CanReferToRawBytes) {
  // (SysEx Non-Realtime Universal ACK to device 22 for receiving packet #61).
  std::uint8_t srcBytes[] = {0xF0, 0x7E, 0x16, 0x7F, 0x3D, 0xF7};
  bmmidi::SysExMsgView sysExMsgView{srcBytes, sizeof(srcBytes)};

  // Can use SysExMsgView accessors:
  EXPECT_THAT(sysExMsgView.sysExId(), Eq(0x7E));
  EXPECT_THAT(sysExMsgView.isUniversal(), IsTrue());

  // Can use non-hidden MsgView accessors:
  EXPECT_THAT(sysExMsgView.type(), Eq(bmmidi::MsgType::kSystemExclusive));
  EXPECT_THAT(sysExMsgView.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kSystemExclusive)));
  EXPECT_THAT(sysExMsgView.numBytes(), Eq(6));
  EXPECT_THAT(sysExMsgView.rawBytes(), Eq(&srcBytes[0]));
}

TEST(SysExMsgView, CanReferToMfrSysEx) {
  auto sysExMsg = bmmidi::MfrSysExBuilder{bmmidi::Manufacturer::nonCommercial()}
      .withNumPayloadBytes(3)
      .buildOnHeap();
  sysExMsg.rawPayloadBytes()[0] = 0xA0;
  sysExMsg.rawPayloadBytes()[1] = 0xA1;
  sysExMsg.rawPayloadBytes()[2] = 0xA2;

  bmmidi::SysExMsgView sysExMsgView{sysExMsg.rawMsgBytes(), sysExMsg.numMsgBytesIncludingEox()};

  // Can use SysExMsgView accessors:
  EXPECT_THAT(sysExMsgView.sysExId(), Eq(0x7D));
  EXPECT_THAT(sysExMsgView.isUniversal(), IsFalse());

  // Can use non-hidden MsgView accessors:
  EXPECT_THAT(sysExMsgView.type(), Eq(bmmidi::MsgType::kSystemExclusive));
  EXPECT_THAT(sysExMsgView.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kSystemExclusive)));
  EXPECT_THAT(sysExMsgView.numBytes(), Eq(6));
  EXPECT_THAT(sysExMsgView.rawBytes(), Eq(sysExMsg.rawMsgBytes()));
}

TEST(SysExMsgView, CanReferToUniversalSysEx) {
  const auto type = bmmidi::universal::kTuningBulkDumpReq;
  const auto device22 = bmmidi::Device::id(22);  // 0x16.

  auto sysExMsg = bmmidi::UniversalSysExBuilder{type, device22}
      .withNumPayloadBytes(1)
      .buildOnHeap();
  sysExMsg.rawPayloadBytes()[0] = 0x04;  // Tuning program #.

  bmmidi::SysExMsgView sysExMsgView{sysExMsg.rawMsgBytes(), sysExMsg.numMsgBytesIncludingEox()};

  // Can use SysExMsgView accessors:
  EXPECT_THAT(sysExMsgView.sysExId(), Eq(0x7E));
  EXPECT_THAT(sysExMsgView.isUniversal(), IsTrue());

  // Can use non-hidden MsgView accessors:
  EXPECT_THAT(sysExMsgView.type(), Eq(bmmidi::MsgType::kSystemExclusive));
  EXPECT_THAT(sysExMsgView.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kSystemExclusive)));
  EXPECT_THAT(sysExMsgView.numBytes(), Eq(7));
  EXPECT_THAT(sysExMsgView.rawBytes(), Eq(sysExMsg.rawMsgBytes()));
}

// NOTE: Not testing SysExMsgRef directly, since there are no SysEx-specific
// mutation functions provided.

TEST(MfrSysExMsgView, WorksForNonCommercialRawBytes) {
  // (Non-Commercial SysEx with 3 payload bytes).
  const std::uint8_t srcBytes[] = {0xF0, 0x7D, 0xA0, 0xA1, 0xA2, 0xF7};
  bmmidi::MfrSysExMsgView mfrMsgView{srcBytes, sizeof(srcBytes)};

  // Can use MfrSysExMsgView accessors:
  EXPECT_THAT(mfrMsgView.manufacturer(), Eq(bmmidi::Manufacturer::nonCommercial()));
  EXPECT_THAT(mfrMsgView.rawPayloadBytes(), Eq(&srcBytes[2]));
  EXPECT_THAT(mfrMsgView.numPayloadBytes(), Eq(3));

  // Can also still use (non-hidden) base accessors:
  EXPECT_THAT(mfrMsgView.sysExId(), Eq(0x7D));
  EXPECT_THAT(mfrMsgView.isUniversal(), IsFalse());

  EXPECT_THAT(mfrMsgView.type(), Eq(bmmidi::MsgType::kSystemExclusive));
  EXPECT_THAT(mfrMsgView.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kSystemExclusive)));
  EXPECT_THAT(mfrMsgView.numBytes(), Eq(6));
  EXPECT_THAT(mfrMsgView.rawBytes(), Eq(&srcBytes[0]));
}

TEST(MfrSysExMsgView, WorksForExtIdMfrSysExMsg) {
  // (Fake Spectrasonics SysEx message format with 3 payload bytes).
  auto sysExMsg = bmmidi::MfrSysExBuilder{bmmidi::Manufacturer::extId(0x02, 0x2C)}
      .withNumPayloadBytes(3)
      .buildOnHeap();
  sysExMsg.rawPayloadBytes()[0] = 0xA0;
  sysExMsg.rawPayloadBytes()[1] = 0xA1;
  sysExMsg.rawPayloadBytes()[2] = 0xA2;

  bmmidi::MfrSysExMsgView mfrMsgView{sysExMsg.rawMsgBytes(), sysExMsg.numMsgBytesIncludingEox()};

  // Can use MfrSysExMsgView accessors:
  EXPECT_THAT(mfrMsgView.manufacturer(), Eq(bmmidi::Manufacturer::extId(0x02, 0x2C)));
  EXPECT_THAT(mfrMsgView.numPayloadBytes(), Eq(3));
  EXPECT_THAT(mfrMsgView.rawPayloadBytes()[0], Eq(0xA0));
  EXPECT_THAT(mfrMsgView.rawPayloadBytes()[1], Eq(0xA1));
  EXPECT_THAT(mfrMsgView.rawPayloadBytes()[2], Eq(0xA2));

  // Can also still use (non-hidden) base accessors:
  EXPECT_THAT(mfrMsgView.sysExId(), Eq(0x00));
  EXPECT_THAT(mfrMsgView.isUniversal(), IsFalse());

  EXPECT_THAT(mfrMsgView.type(), Eq(bmmidi::MsgType::kSystemExclusive));
  EXPECT_THAT(mfrMsgView.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kSystemExclusive)));
  EXPECT_THAT(mfrMsgView.numBytes(), Eq(8));
  EXPECT_THAT(mfrMsgView.rawBytes()[0], Eq(0xF0));  // SysEx status byte.
  EXPECT_THAT(mfrMsgView.rawBytes()[1], Eq(0x00));  // SysEx ID (extended manufacturer ID).
  EXPECT_THAT(mfrMsgView.rawBytes()[2], Eq(0x02));  // Extended Manufacturer ID byte #1.
  EXPECT_THAT(mfrMsgView.rawBytes()[3], Eq(0x2C));  // Extended Manufacturer ID byte #2.

  EXPECT_THAT(mfrMsgView.rawBytes()[4], Eq(0xA0));  // Payload [0]
  EXPECT_THAT(mfrMsgView.rawBytes()[5], Eq(0xA1));  // Payload [1]
  EXPECT_THAT(mfrMsgView.rawBytes()[6], Eq(0xA2));  // Payload [2]

  EXPECT_THAT(mfrMsgView.rawBytes()[7], Eq(0xF7));  // EOX.
}

TEST(MfrSysExMsgRef, WorksForShortIdRawBytes) {
  // (Fake Sequential Circuits SysEx message format with 4 payload bytes).
  std::uint8_t srcBytes[] = {0xF0, 0x01, 0xA0, 0xA1, 0xA2, 0xA3, 0xF7};
  bmmidi::MfrSysExMsgRef mfrMsgRef{srcBytes, sizeof(srcBytes)};

  // Can use MfrSysExMsgRef accessors:
  EXPECT_THAT(mfrMsgRef.manufacturer(), Eq(bmmidi::Manufacturer::shortId(0x01)));
  EXPECT_THAT(mfrMsgRef.rawPayloadBytes(), Eq(&srcBytes[2]));
  EXPECT_THAT(mfrMsgRef.numPayloadBytes(), Eq(4));

  // Can also still use (non-hidden) base accessors:
  EXPECT_THAT(mfrMsgRef.sysExId(), Eq(0x01));
  EXPECT_THAT(mfrMsgRef.isUniversal(), IsFalse());

  EXPECT_THAT(mfrMsgRef.type(), Eq(bmmidi::MsgType::kSystemExclusive));
  EXPECT_THAT(mfrMsgRef.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kSystemExclusive)));
  EXPECT_THAT(mfrMsgRef.numBytes(), Eq(7));
  EXPECT_THAT(mfrMsgRef.rawBytes(), Eq(&srcBytes[0]));

  // Can mutate payload bytes only (other mutators aren't provided, since they
  // could end up changing the # of message bytes).
  mfrMsgRef.rawPayloadBytes()[0] = 0xB0;
  mfrMsgRef.rawPayloadBytes()[1] = 0xB1;
  mfrMsgRef.rawPayloadBytes()[2] = 0xB2;
  mfrMsgRef.rawPayloadBytes()[3] = 0xB3;

  EXPECT_THAT(srcBytes[2], Eq(0xB0));
  EXPECT_THAT(srcBytes[3], Eq(0xB1));
  EXPECT_THAT(srcBytes[4], Eq(0xB2));
  EXPECT_THAT(srcBytes[5], Eq(0xB3));
}

TEST(UniversalSysExMsgView, WorksForOneSubIdTypeRawBytes) {
  // (Non-Realtime Universal SysEx ACK to device 22 for packet #61).
  const std::uint8_t srcBytes[] = {0xF0, 0x7E, 0x16, 0x7F, 0x3D, 0xF7};
  bmmidi::UniversalSysExMsgView univMsgView{srcBytes, sizeof(srcBytes)};

  // Can use UniversalSysExMsgView accessors:
  EXPECT_THAT(univMsgView.category(), Eq(bmmidi::UniversalCategory::kNonRealTime));
  EXPECT_THAT(univMsgView.universalType(), Eq(bmmidi::universal::kAck));
  EXPECT_THAT(univMsgView.device(), Eq(bmmidi::Device::id(22)));
  EXPECT_THAT(univMsgView.numPayloadBytes(), Eq(1));
  EXPECT_THAT(univMsgView.rawPayloadBytes(), Eq(&srcBytes[4]));
  EXPECT_THAT(univMsgView.rawPayloadBytes()[0], Eq(0x3D));  // Device 22.

  // Can also still use (non-hidden) base accessors:
  EXPECT_THAT(univMsgView.sysExId(), Eq(0x7E));
  EXPECT_THAT(univMsgView.isUniversal(), IsTrue());

  EXPECT_THAT(univMsgView.type(), Eq(bmmidi::MsgType::kSystemExclusive));
  EXPECT_THAT(univMsgView.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kSystemExclusive)));
  EXPECT_THAT(univMsgView.numBytes(), Eq(6));
  EXPECT_THAT(univMsgView.rawBytes(), Eq(&srcBytes[0]));
}

TEST(UniversalSysExMsgView, WorksForRealtimeRawBytes) {
  // (Realtime Universal SysEx Tuning Note Change for device 22, tuning program 0x04, key 60).
  const std::uint8_t srcBytes[] =
      {0xF0, 0x7F, 0x16, 0x08, 0x02, 0x04, 0x01, 0x3C, 0x3B, 0x63, 0x1A, 0xF7};
  bmmidi::UniversalSysExMsgView univMsgView{srcBytes, sizeof(srcBytes)};

  // Can use UniversalSysExMsgView accessors:
  EXPECT_THAT(univMsgView.category(), Eq(bmmidi::UniversalCategory::kRealTime));
  EXPECT_THAT(univMsgView.universalType(), Eq(bmmidi::universal::kTuningRtNoteChange));
  EXPECT_THAT(univMsgView.device(), Eq(bmmidi::Device::id(22)));
  EXPECT_THAT(univMsgView.numPayloadBytes(), Eq(6));
  EXPECT_THAT(univMsgView.rawPayloadBytes(), Eq(&srcBytes[5]));

  // Can also still use (non-hidden) base accessors:
  EXPECT_THAT(univMsgView.sysExId(), Eq(0x7F));
  EXPECT_THAT(univMsgView.isUniversal(), IsTrue());

  EXPECT_THAT(univMsgView.type(), Eq(bmmidi::MsgType::kSystemExclusive));
  EXPECT_THAT(univMsgView.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kSystemExclusive)));
  EXPECT_THAT(univMsgView.numBytes(), Eq(12));
  EXPECT_THAT(univMsgView.rawBytes(), Eq(&srcBytes[0]));

  EXPECT_THAT(univMsgView.rawBytes()[0], Eq(0xF0));  // SysEx status byte.
  EXPECT_THAT(univMsgView.rawBytes()[1], Eq(0x7F));  // SysEx ID (realtime universal).
  EXPECT_THAT(univMsgView.rawBytes()[2], Eq(0x16));  // Device ID.
  EXPECT_THAT(univMsgView.rawBytes()[3], Eq(0x08));  // Sub-ID #1 (0x08).
  EXPECT_THAT(univMsgView.rawBytes()[4], Eq(0x02));  // Sub-ID #2 (0x02).

  EXPECT_THAT(univMsgView.rawBytes()[5], Eq(0x04));  // Payload [0] (tuning program #).
  EXPECT_THAT(univMsgView.rawBytes()[6], Eq(0x01));  // Payload [1] (# notes).
  EXPECT_THAT(univMsgView.rawBytes()[7], Eq(0x3C));  // Payload [2] (key 60).
  EXPECT_THAT(univMsgView.rawBytes()[8], Eq(0x3B));  // Payload [3] (key 59 below target freq).

  // Next 2 bytes are fraction of 100 cents above the previous MIDI key,
  // here 3427 * 100 / (2^14) = 20.916748... cents.
  // With A4=440 Hz global tuning, the resultant frequency would be 249.94329294... Hz.
  // LSB:            0b110 0011 (  0x63 = 99)
  // MSB:   0b00 1101 0         (  0x1A = 26)
  // Val: 0b  00 1101 0110 0011 (0x0D63 = 3427)
  EXPECT_THAT(univMsgView.rawBytes()[9], Eq(0x63));  // Payload [4] (LSB fraction above)
  EXPECT_THAT(univMsgView.rawBytes()[10], Eq(0x1A)); // Payload [5] (MSB fraction above)

  EXPECT_THAT(univMsgView.rawBytes()[11], Eq(0xF7)); // EOX.
}

TEST(UniversalSysExMsgRef, WorksForTwoSubIdTypeUniversalSysExMsg) {
  // (Non-Realtime Universal SysEx Tuning Bulk Dump Request, all devices, tuning program 0x04).
  const auto type = bmmidi::universal::kTuningBulkDumpReq;
  const auto allDevices = bmmidi::Device::all();

  auto sysExMsg = bmmidi::UniversalSysExBuilder{type, allDevices}
      .withNumPayloadBytes(1)
      .buildOnHeap();
  sysExMsg.rawPayloadBytes()[0] = 0x04;

  bmmidi::UniversalSysExMsgRef univMsgRef{
      sysExMsg.rawMsgBytes(), sysExMsg.numMsgBytesIncludingEox()};

  // Can use UniversalSysExMsgRef accessors:
  EXPECT_THAT(univMsgRef.category(), Eq(bmmidi::UniversalCategory::kNonRealTime));
  EXPECT_THAT(univMsgRef.universalType(), Eq(bmmidi::universal::kTuningBulkDumpReq));
  EXPECT_THAT(univMsgRef.device(), Eq(bmmidi::Device::all()));
  EXPECT_THAT(univMsgRef.numPayloadBytes(), Eq(1));
  EXPECT_THAT(univMsgRef.rawPayloadBytes()[0], Eq(0x04));  // Tuning program.

  // Can also still use (non-hidden) base accessors:
  EXPECT_THAT(univMsgRef.sysExId(), Eq(0x7E));
  EXPECT_THAT(univMsgRef.isUniversal(), IsTrue());

  EXPECT_THAT(univMsgRef.type(), Eq(bmmidi::MsgType::kSystemExclusive));
  EXPECT_THAT(univMsgRef.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kSystemExclusive)));
  EXPECT_THAT(univMsgRef.numBytes(), Eq(7));
  EXPECT_THAT(univMsgRef.rawBytes()[0], Eq(0xF0));  // SysEx status byte.
  EXPECT_THAT(univMsgRef.rawBytes()[1], Eq(0x7E));  // SysEx ID (non-realtime universal).
  EXPECT_THAT(univMsgRef.rawBytes()[2], Eq(0x7F));  // Device ID ("all").
  EXPECT_THAT(univMsgRef.rawBytes()[3], Eq(0x08));  // Sub-ID #1 (0x08).
  EXPECT_THAT(univMsgRef.rawBytes()[4], Eq(0x00));  // Sub-ID #2 (0x00).

  EXPECT_THAT(univMsgRef.rawBytes()[5], Eq(0x04));  // Payload [0] (tuning program #).

  EXPECT_THAT(univMsgRef.rawBytes()[6], Eq(0xF7));  // EOX.

  // Can mutate device and payload bytes only (other mutators aren't provided,
  // since they could end up changing the # of message bytes).
  univMsgRef.setDevice(bmmidi::Device::id(22));  // 0x16.
  univMsgRef.rawPayloadBytes()[0] = 0x3D;  // Tuning program.

  EXPECT_THAT(univMsgRef.rawBytes()[0], Eq(0xF0));  // SysEx status byte.
  EXPECT_THAT(univMsgRef.rawBytes()[1], Eq(0x7E));  // SysEx ID (non-realtime universal).
  EXPECT_THAT(univMsgRef.rawBytes()[2], Eq(0x16));  // Device ID (22).
  EXPECT_THAT(univMsgRef.rawBytes()[3], Eq(0x08));  // Sub-ID #1 (0x08).
  EXPECT_THAT(univMsgRef.rawBytes()[4], Eq(0x00));  // Sub-ID #2 (0x00).

  EXPECT_THAT(univMsgRef.rawBytes()[5], Eq(0x3D));  // Payload [0] (tuning program #).

  EXPECT_THAT(univMsgRef.rawBytes()[6], Eq(0xF7));  // EOX.
}

TEST(MtcQuarterFrameMsgView, CanReferToRawBytes) {
  // (MTC Quarter Frame, MM lower 4 bits of 0b1011).
  // Upper bits identifying piece are 100. Lower bits encode value (for MM lower 4 bits).
  const std::uint8_t srcBytes[] = {0xF1, 0b0100'1011};  // 0x4B.
  bmmidi::MtcQuarterFrameMsgView qfMsgView{srcBytes, sizeof(srcBytes)};

  // Can use MtcQuarterFrameMsgView accessors:
  EXPECT_THAT(qfMsgView.dataByte(), Eq(0x4B));
  EXPECT_THAT(qfMsgView.piece(), Eq(bmmidi::MtcQuarterFramePiece::k4MinLowerBits));
  EXPECT_THAT(qfMsgView.valueInLower4Bits(), Eq(0x0B));

  // Can also still use base accessors:
  EXPECT_THAT(qfMsgView.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kMtcQuarterFrame)));
  EXPECT_THAT(qfMsgView.data1(), Eq(bmmidi::DataValue{0x4B}));
}

TEST(MtcQuarterFrameMsgRef, CanReferToRawBytes) {
  // (MTC Quarter Frame, MM lower 4 bits of 0b1011).
  // Upper bits identifying piece are 100. Lower bits encode value (for MM lower 4 bits).
  std::uint8_t srcBytes[] = {0xF1, 0b0100'1011};  // 0x4B.
  bmmidi::MtcQuarterFrameMsgRef qfMsgRef{srcBytes, sizeof(srcBytes)};

  // Can use MtcQuarterFrameMsgRef accessors:
  EXPECT_THAT(qfMsgRef.dataByte(), Eq(0x4B));
  EXPECT_THAT(qfMsgRef.piece(), Eq(bmmidi::MtcQuarterFramePiece::k4MinLowerBits));
  EXPECT_THAT(qfMsgRef.valueInLower4Bits(), Eq(0x0B));

  // Can also still use base accessors:
  EXPECT_THAT(qfMsgRef.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kMtcQuarterFrame)));
  EXPECT_THAT(qfMsgRef.data1(), Eq(bmmidi::DataValue{0x4B}));

  // Can mutate whole data byte:
  // (Rate + HH 0rrh upper 4 bits of 0b0011).
  qfMsgRef.setDataByte(0b0111'0011);  // 0x73.

  EXPECT_THAT(qfMsgRef.dataByte(), Eq(0x73));
  EXPECT_THAT(qfMsgRef.piece(), Eq(bmmidi::MtcQuarterFramePiece::k7RateHourUpperBits));
  EXPECT_THAT(qfMsgRef.valueInLower4Bits(), Eq(0x03));
  EXPECT_THAT(qfMsgRef.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kMtcQuarterFrame)));
  EXPECT_THAT(qfMsgRef.data1(), Eq(bmmidi::DataValue{0x73}));

  EXPECT_THAT(srcBytes[0], Eq(0xF1));
  EXPECT_THAT(srcBytes[1], Eq(0x73));

  // Or can set piece and value individually:
  // (FF upper 1 bit of 0b0001).
  qfMsgRef.setPiece(bmmidi::MtcQuarterFramePiece::k1FrameUpperBits);  // 0x10.
  qfMsgRef.setValueFromLower4Bits(0b0000'0001);  // 0x01.

  EXPECT_THAT(qfMsgRef.dataByte(), Eq(0x11));
  EXPECT_THAT(qfMsgRef.piece(), Eq(bmmidi::MtcQuarterFramePiece::k1FrameUpperBits));
  EXPECT_THAT(qfMsgRef.valueInLower4Bits(), Eq(0x01));
  EXPECT_THAT(qfMsgRef.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kMtcQuarterFrame)));
  EXPECT_THAT(qfMsgRef.data1(), Eq(bmmidi::DataValue{0x11}));

  EXPECT_THAT(srcBytes[0], Eq(0xF1));
  EXPECT_THAT(srcBytes[1], Eq(0x11));
}

TEST(SongPosMsgView, CanReferToRawBytes) {
  // (Song Position Pointer, 15243 sixteenths after start).
  // LSB:            0b000 1011 (  0x0B = 11)
  // MSB:   0b11 1011 1         (  0x77 = 119)
  // Val: 0b  11 1011 1000 1011 (0x3B8B = 15243)
  const std::uint8_t srcBytes[] = {0xF2, 0x0B, 0x77};
  bmmidi::SongPosMsgView posMsgView{srcBytes, sizeof(srcBytes)};

  // Can use SongPosMsgView accessors:
  EXPECT_THAT(posMsgView.sixteenthsAfterStart().value(), Eq(15243));
  EXPECT_THAT(posMsgView.sixteenthsAfterStart().lsb(), Eq(0x0B));
  EXPECT_THAT(posMsgView.sixteenthsAfterStart().msb(), Eq(0x77));

  // Can also still use base accessors:
  EXPECT_THAT(posMsgView.status(),
      Eq(bmmidi::Status::system(bmmidi::MsgType::kSongPositionPointer)));
  EXPECT_THAT(posMsgView.data1(), Eq(bmmidi::DataValue{0x0B}));
  EXPECT_THAT(posMsgView.data2(), Eq(bmmidi::DataValue{0x77}));
}

TEST(SongPosMsgRef, CanReferToRawBytes) {
  // (Song Position Pointer, 15243 sixteenths after start).
  // LSB:            0b000 1011 (  0x0B = 11)
  // MSB:   0b11 1011 1         (  0x77 = 119)
  // Val: 0b  11 1011 1000 1011 (0x3B8B = 15243)
  std::uint8_t srcBytes[] = {0xF2, 0x0B, 0x77};
  bmmidi::SongPosMsgRef posMsgRef{srcBytes, sizeof(srcBytes)};

  // Can use SongPosMsgRef accessors:
  EXPECT_THAT(posMsgRef.sixteenthsAfterStart().value(), Eq(15243));
  EXPECT_THAT(posMsgRef.sixteenthsAfterStart().lsb(), Eq(0x0B));
  EXPECT_THAT(posMsgRef.sixteenthsAfterStart().msb(), Eq(0x77));

  // Can also still use base accessors:
  EXPECT_THAT(posMsgRef.status(),
      Eq(bmmidi::Status::system(bmmidi::MsgType::kSongPositionPointer)));
  EXPECT_THAT(posMsgRef.data1(), Eq(bmmidi::DataValue{0x0B}));
  EXPECT_THAT(posMsgRef.data2(), Eq(bmmidi::DataValue{0x77}));

  // Can mutate:
  posMsgRef.setSixteenthsAfterStart(bmmidi::DoubleDataValue::min());

  EXPECT_THAT(posMsgRef.sixteenthsAfterStart().value(), Eq(0));
  EXPECT_THAT(posMsgRef.sixteenthsAfterStart().lsb(), Eq(0x00));
  EXPECT_THAT(posMsgRef.sixteenthsAfterStart().msb(), Eq(0x00));
  EXPECT_THAT(posMsgRef.status(),
      Eq(bmmidi::Status::system(bmmidi::MsgType::kSongPositionPointer)));
  EXPECT_THAT(posMsgRef.data1(), Eq(bmmidi::DataValue{0x00}));
  EXPECT_THAT(posMsgRef.data2(), Eq(bmmidi::DataValue{0x00}));

  EXPECT_THAT(srcBytes[0], Eq(0xF2));
  EXPECT_THAT(srcBytes[1], Eq(0x00));
  EXPECT_THAT(srcBytes[2], Eq(0x00));
}

TEST(SongSelectMsgView, CanReferToRawBytes) {
  // (Song Select, song 57).
  const std::uint8_t srcBytes[] = {0xF3, 0x38};
  bmmidi::SongSelectMsgView songMsgView{srcBytes, sizeof(srcBytes)};

  // Can use SongSelectMsgView accessors:
  EXPECT_THAT(songMsgView.song().displayNumber(), Eq(57));

  // Can also still use base accessors:
  EXPECT_THAT(songMsgView.status(),
      Eq(bmmidi::Status::system(bmmidi::MsgType::kSongSelect)));
  EXPECT_THAT(songMsgView.data1(), Eq(bmmidi::DataValue{56}));
}

TEST(SongSelectMsgRef, CanReferToRawBytes) {
  // (Song Select, song 57).
  std::uint8_t srcBytes[] = {0xF3, 0x38};
  bmmidi::SongSelectMsgRef songMsgRef{srcBytes, sizeof(srcBytes)};

  // Can use SongSelectMsgRef accessors:
  EXPECT_THAT(songMsgRef.song().displayNumber(), Eq(57));

  // Can also still use base accessors:
  EXPECT_THAT(songMsgRef.status(),
      Eq(bmmidi::Status::system(bmmidi::MsgType::kSongSelect)));
  EXPECT_THAT(songMsgRef.data1(), Eq(bmmidi::DataValue{56}));

  // Can mutate:
  songMsgRef.setSong(bmmidi::PresetNumber::index(22));

  EXPECT_THAT(songMsgRef.song().displayNumber(), Eq(23));
  EXPECT_THAT(songMsgRef.status(),
      Eq(bmmidi::Status::system(bmmidi::MsgType::kSongSelect)));
  EXPECT_THAT(songMsgRef.data1(), Eq(bmmidi::DataValue{22}));

  EXPECT_THAT(srcBytes[0], Eq(0xF3));
  EXPECT_THAT(srcBytes[1], Eq(0x16));
}

}  // namespace
