#include "bmmidi/msg.hpp"

#include <cstdint>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bmmidi/channel.hpp"
#include "bmmidi/control.hpp"
#include "bmmidi/data_value.hpp"
#include "bmmidi/status.hpp"

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

  // Can use ControlChangeMsgView accessors:
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

  // Can use ProgramChangeMsgView accessors:
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

TEST(Msg, ProvidesNumBytesConstant) {
  EXPECT_THAT(bmmidi::Msg<1>::kNumBytes, Eq(1));
  EXPECT_THAT(bmmidi::Msg<2>::kNumBytes, Eq(2));
  EXPECT_THAT(bmmidi::Msg<3>::kNumBytes, Eq(3));
}

TEST(Msg, OneByteVersionHasStatus) {
  bmmidi::Msg<1> msg{bmmidi::Status::system(bmmidi::MsgType::kOscillatorTuneRequest)};

  EXPECT_THAT(msg.type(), Eq(bmmidi::MsgType::kOscillatorTuneRequest));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::system(bmmidi::MsgType::kOscillatorTuneRequest)));

  msg.setStatus(bmmidi::Status::system(bmmidi::MsgType::kSystemReset));
  EXPECT_THAT(msg.type(), Eq(bmmidi::MsgType::kSystemReset));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::system(bmmidi::MsgType::kSystemReset)));
}

TEST(Msg, TwoByteVersionHasData1) {
  bmmidi::Msg<2> msg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                   bmmidi::Channel::index(12)),
      bmmidi::DataValue{57}};

  EXPECT_THAT(msg.type(), Eq(bmmidi::MsgType::kProgramChange));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                      bmmidi::Channel::index(12))));
  EXPECT_THAT(msg.data1(), Eq(bmmidi::DataValue{57}));

  msg.setData1(bmmidi::DataValue{63});
  EXPECT_THAT(msg.type(), Eq(bmmidi::MsgType::kProgramChange));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                      bmmidi::Channel::index(12))));
  EXPECT_THAT(msg.data1(), Eq(bmmidi::DataValue{63}));

  msg.setStatus(
      bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                   bmmidi::Channel::index(3)));
  EXPECT_THAT(msg.type(), Eq(bmmidi::MsgType::kChannelPressure));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                      bmmidi::Channel::index(3))));
  EXPECT_THAT(msg.data1(), Eq(bmmidi::DataValue{63}));
}

TEST(Msg, ThreeByteVersionHasData2) {
  bmmidi::Msg<3> msg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(9)),
      bmmidi::controlToDataValue(bmmidi::Control::kExpression),
      bmmidi::DataValue{76}};

  EXPECT_THAT(msg.type(), Eq(bmmidi::MsgType::kControlChange));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(9))));
  EXPECT_THAT(msg.data1(), Eq(bmmidi::DataValue{11}));
  EXPECT_THAT(msg.data2(), Eq(bmmidi::DataValue{76}));

  msg.setData2(bmmidi::DataValue{27});
  EXPECT_THAT(msg.type(), Eq(bmmidi::MsgType::kControlChange));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(9))));
  EXPECT_THAT(msg.data1(), Eq(bmmidi::DataValue{11}));
  EXPECT_THAT(msg.data2(), Eq(bmmidi::DataValue{27}));

  msg.setData1(bmmidi::controlToDataValue(bmmidi::Control::kModWheel));
  EXPECT_THAT(msg.type(), Eq(bmmidi::MsgType::kControlChange));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(9))));
  EXPECT_THAT(msg.data1(), Eq(bmmidi::DataValue{1}));
  EXPECT_THAT(msg.data2(), Eq(bmmidi::DataValue{27}));

  msg.setStatus(
      bmmidi::Status::channelVoice(bmmidi::MsgType::kPolyphonicKeyPressure,
                                   bmmidi::Channel::index(3)));
  EXPECT_THAT(msg.type(), Eq(bmmidi::MsgType::kPolyphonicKeyPressure));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kPolyphonicKeyPressure,
                                      bmmidi::Channel::index(3))));
  EXPECT_THAT(msg.data1(), Eq(bmmidi::DataValue{1}));
  EXPECT_THAT(msg.data2(), Eq(bmmidi::DataValue{27}));
}

TEST(Msg, OneByteVersionSupportsEqualityOperations) {
  bmmidi::Msg<1> oscTuneRequest{
      bmmidi::Status::system(bmmidi::MsgType::kOscillatorTuneRequest)};
  bmmidi::Msg<1> alsoOscTuneRequest{
      bmmidi::Status::system(bmmidi::MsgType::kOscillatorTuneRequest)};
  bmmidi::Msg<1> systemReset{
      bmmidi::Status::system(bmmidi::MsgType::kSystemReset)};

  EXPECT_THAT(oscTuneRequest == oscTuneRequest, IsTrue());
  EXPECT_THAT(oscTuneRequest != oscTuneRequest, IsFalse());

  EXPECT_THAT(oscTuneRequest == alsoOscTuneRequest, IsTrue());
  EXPECT_THAT(alsoOscTuneRequest == oscTuneRequest, IsTrue());
  EXPECT_THAT(oscTuneRequest != alsoOscTuneRequest, IsFalse());
  EXPECT_THAT(alsoOscTuneRequest != oscTuneRequest, IsFalse());

  EXPECT_THAT(oscTuneRequest == systemReset, IsFalse());
  EXPECT_THAT(systemReset == oscTuneRequest, IsFalse());
  EXPECT_THAT(oscTuneRequest != systemReset, IsTrue());
  EXPECT_THAT(systemReset != oscTuneRequest, IsTrue());
}

TEST(Msg, TwoByteVersionSupportsEqualityOperations) {
  bmmidi::Msg<2> pcCh13Prog57{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                   bmmidi::Channel::index(12)),
      bmmidi::DataValue{57}};
  bmmidi::Msg<2> alsoProgramChangeCh13Prog57{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                   bmmidi::Channel::index(12)),
      bmmidi::DataValue{57}};
  bmmidi::Msg<2> pcCh13Prog58{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                   bmmidi::Channel::index(12)),
      bmmidi::DataValue{58}};

  EXPECT_THAT(pcCh13Prog57 == pcCh13Prog57, IsTrue());
  EXPECT_THAT(pcCh13Prog57 != pcCh13Prog57, IsFalse());

  EXPECT_THAT(pcCh13Prog57 == alsoProgramChangeCh13Prog57, IsTrue());
  EXPECT_THAT(alsoProgramChangeCh13Prog57 == pcCh13Prog57, IsTrue());
  EXPECT_THAT(pcCh13Prog57 != alsoProgramChangeCh13Prog57, IsFalse());
  EXPECT_THAT(alsoProgramChangeCh13Prog57 != pcCh13Prog57, IsFalse());

  EXPECT_THAT(pcCh13Prog57 == pcCh13Prog58, IsFalse());
  EXPECT_THAT(pcCh13Prog58 == pcCh13Prog57, IsFalse());
  EXPECT_THAT(pcCh13Prog57 != pcCh13Prog58, IsTrue());
  EXPECT_THAT(pcCh13Prog58 != pcCh13Prog57, IsTrue());
}

TEST(Msg, ThreeByteVersionSupportsEqualityOperations) {
  bmmidi::Msg<3> ccCh10Expr76{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(9)),
      bmmidi::controlToDataValue(bmmidi::Control::kExpression),
      bmmidi::DataValue{76}};
  bmmidi::Msg<3> alsoCcCh10Expr76{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(9)),
      bmmidi::controlToDataValue(bmmidi::Control::kExpression),
      bmmidi::DataValue{76}};
  bmmidi::Msg<3> ccCh10Expr77{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(9)),
      bmmidi::controlToDataValue(bmmidi::Control::kExpression),
      bmmidi::DataValue{77}};

  EXPECT_THAT(ccCh10Expr76 == ccCh10Expr76, IsTrue());
  EXPECT_THAT(ccCh10Expr76 != ccCh10Expr76, IsFalse());

  EXPECT_THAT(ccCh10Expr76 == alsoCcCh10Expr76, IsTrue());
  EXPECT_THAT(alsoCcCh10Expr76 == ccCh10Expr76, IsTrue());
  EXPECT_THAT(ccCh10Expr76 != alsoCcCh10Expr76, IsFalse());
  EXPECT_THAT(alsoCcCh10Expr76 != ccCh10Expr76, IsFalse());

  EXPECT_THAT(ccCh10Expr76 == ccCh10Expr77, IsFalse());
  EXPECT_THAT(ccCh10Expr77 == ccCh10Expr76, IsFalse());
  EXPECT_THAT(ccCh10Expr76 != ccCh10Expr77, IsTrue());
  EXPECT_THAT(ccCh10Expr77 != ccCh10Expr76, IsTrue());
}

TEST(Msg, ProvidesReadOnlyRawArrayAccess) {
  // One byte version:
  const bmmidi::Msg<1> oscTuneRequest{
      bmmidi::Status::system(bmmidi::MsgType::kOscillatorTuneRequest)};
  EXPECT_THAT(sizeof(oscTuneRequest.rawBytes()), Eq(1));
  EXPECT_THAT(oscTuneRequest.rawBytes()[0], Eq(std::uint8_t{0xF6}));

  // Two byte version:
  const bmmidi::Msg<2> programChangeCh13{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                   bmmidi::Channel::index(12)),
      bmmidi::DataValue{57}};
  EXPECT_THAT(sizeof(programChangeCh13.rawBytes()), Eq(2));
  EXPECT_THAT(programChangeCh13.rawBytes()[0], Eq(std::uint8_t{0xCC}));
  EXPECT_THAT(programChangeCh13.rawBytes()[1], Eq(std::uint8_t{0x39}));

  // Three byte version:
  const bmmidi::Msg<3> controlChangeCh10{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(9)),
      bmmidi::controlToDataValue(bmmidi::Control::kExpression),
      bmmidi::DataValue{76}};
  EXPECT_THAT(sizeof(controlChangeCh10.rawBytes()), Eq(3));
  EXPECT_THAT(controlChangeCh10.rawBytes()[0], Eq(std::uint8_t{0xB9}));
  EXPECT_THAT(controlChangeCh10.rawBytes()[1], Eq(std::uint8_t{0x0B}));
  EXPECT_THAT(controlChangeCh10.rawBytes()[2], Eq(std::uint8_t{0x4C}));
}

TEST(Msg, ProvidesReadWriteRawArrayAccess) {
  bmmidi::Msg<3> msg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(9)),
      bmmidi::controlToDataValue(bmmidi::Control::kExpression),
      bmmidi::DataValue{76}};
  EXPECT_THAT(sizeof(msg.rawBytes()), Eq(3));
  EXPECT_THAT(msg.rawBytes()[0], Eq(std::uint8_t{0xB9}));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(9))));
  EXPECT_THAT(msg.rawBytes()[1], Eq(std::uint8_t{0x0B}));
  EXPECT_THAT(msg.data1(), Eq(bmmidi::DataValue{11}));
  EXPECT_THAT(msg.rawBytes()[2], Eq(std::uint8_t{0x4C}));
  EXPECT_THAT(msg.data2(), Eq(bmmidi::DataValue{76}));

  msg.rawBytes()[2] = 0x1B;
  EXPECT_THAT(msg.rawBytes()[0], Eq(std::uint8_t{0xB9}));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(9))));
  EXPECT_THAT(msg.rawBytes()[1], Eq(std::uint8_t{0x0B}));
  EXPECT_THAT(msg.data1(), Eq(bmmidi::DataValue{11}));
  EXPECT_THAT(msg.rawBytes()[2], Eq(std::uint8_t{0x1B}));
  EXPECT_THAT(msg.data2(), Eq(bmmidi::DataValue{27}));

  msg.rawBytes()[1] = 0x01;
  EXPECT_THAT(msg.rawBytes()[0], Eq(std::uint8_t{0xB9}));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(9))));
  EXPECT_THAT(msg.rawBytes()[1], Eq(std::uint8_t{0x01}));
  EXPECT_THAT(msg.data1(), Eq(bmmidi::DataValue{1}));
  EXPECT_THAT(msg.rawBytes()[2], Eq(std::uint8_t{0x1B}));
  EXPECT_THAT(msg.data2(), Eq(bmmidi::DataValue{27}));

  msg.rawBytes()[0] = 0xA3;
  EXPECT_THAT(msg.rawBytes()[0], Eq(std::uint8_t{0xA3}));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kPolyphonicKeyPressure,
                                      bmmidi::Channel::index(3))));
  EXPECT_THAT(msg.rawBytes()[1], Eq(std::uint8_t{0x01}));
  EXPECT_THAT(msg.data1(), Eq(bmmidi::DataValue{1}));
  EXPECT_THAT(msg.rawBytes()[2], Eq(std::uint8_t{0x1B}));
  EXPECT_THAT(msg.data2(), Eq(bmmidi::DataValue{27}));
}

TEST(ChanMsg, SupportsTwoByteMsgs) {
  // (Program change, channel 13, program 57).
  bmmidi::ChanMsg<2> chanMsg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                   bmmidi::Channel::index(12)),
      bmmidi::DataValue{57}};
  
  // Can use channel accessor:
  EXPECT_THAT(chanMsg.channel().displayNumber(), Eq(13));

  // Can also still use Msg accessors:
  EXPECT_THAT(chanMsg.type(), Eq(bmmidi::MsgType::kProgramChange));
  EXPECT_THAT(chanMsg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                      bmmidi::Channel::index(12))));
  EXPECT_THAT(chanMsg.data1(), Eq(bmmidi::DataValue{57}));

  // Can mutate channel:
  chanMsg.setChannel(bmmidi::Channel::index(2));
  
  EXPECT_THAT(chanMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(chanMsg.type(), Eq(bmmidi::MsgType::kProgramChange));
  EXPECT_THAT(chanMsg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(chanMsg.data1(), Eq(bmmidi::DataValue{57}));
}

TEST(ChanMsg, SupportsThreeByteMsgs) {
  // (Note On, channel 3, key 69, velocity 90).
  bmmidi::ChanMsg<3> chanMsg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                   bmmidi::Channel::index(2)),
      bmmidi::DataValue{69},
      bmmidi::DataValue{90}};

  // Can use channel accessor:
  EXPECT_THAT(chanMsg.channel().displayNumber(), Eq(3));

  // Can also still use Msg accessors:
  EXPECT_THAT(chanMsg.type(), Eq(bmmidi::MsgType::kNoteOn));
  EXPECT_THAT(chanMsg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(chanMsg.data1(), Eq(bmmidi::DataValue{69}));
  EXPECT_THAT(chanMsg.data2(), Eq(bmmidi::DataValue{90}));

  // Can mutate channel:
  chanMsg.setChannel(bmmidi::Channel::index(13));
  
  EXPECT_THAT(chanMsg.channel().displayNumber(), Eq(14));
  EXPECT_THAT(chanMsg.type(), Eq(bmmidi::MsgType::kNoteOn));
  EXPECT_THAT(chanMsg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                      bmmidi::Channel::index(13))));
  EXPECT_THAT(chanMsg.data1(), Eq(bmmidi::DataValue{69}));
  EXPECT_THAT(chanMsg.data2(), Eq(bmmidi::DataValue{90}));
}

TEST(ChanMsg, ShouldConvertFromTwoByteMsg) {
  // A more generic Msg<2>...
  // (Program change, channel 13, program 57).
  bmmidi::Msg<2> msg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                   bmmidi::Channel::index(12)),
      bmmidi::DataValue{57}};
  
  // ...should convert to the more specific ChanMsg<2>...
  auto chanMsg = msg.to<bmmidi::ChanMsg<2>>();

  EXPECT_THAT(chanMsg.channel().displayNumber(), Eq(13));
  EXPECT_THAT(chanMsg.type(), Eq(bmmidi::MsgType::kProgramChange));
  EXPECT_THAT(chanMsg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                      bmmidi::Channel::index(12))));
  EXPECT_THAT(chanMsg.data1(), Eq(bmmidi::DataValue{57}));

  // ...and the new ChanMsg<2> copy should be mutable...
  chanMsg.setChannel(bmmidi::Channel::index(2));
  EXPECT_THAT(chanMsg.channel().displayNumber(), Eq(3));

  // ...without affecting the original Msg<2>.
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                      bmmidi::Channel::index(12))));
}

TEST(ChanMsg, ShouldConvertFromThreeByteMsg) {
  // A more generic Msg<3>...
  // (Note On, channel 3, key 69, velocity 90).
  bmmidi::Msg<3> msg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                   bmmidi::Channel::index(2)),
      bmmidi::DataValue{69},
      bmmidi::DataValue{90}};
  
  // ...should convert to the more specific ChanMsg<3>...
  auto chanMsg = msg.to<bmmidi::ChanMsg<3>>();

  EXPECT_THAT(chanMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(chanMsg.type(), Eq(bmmidi::MsgType::kNoteOn));
  EXPECT_THAT(chanMsg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(chanMsg.data1(), Eq(bmmidi::DataValue{69}));
  EXPECT_THAT(chanMsg.data2(), Eq(bmmidi::DataValue{90}));

  // ...and the new ChanMsg<3> copy should be mutable...
  chanMsg.setChannel(bmmidi::Channel::index(13));
  EXPECT_THAT(chanMsg.channel().displayNumber(), Eq(14));

  // ...without affecting the original Msg<3>.
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                      bmmidi::Channel::index(2))));
}

TEST(NoteMsg, ShouldCreateNoteOn) {
  // (Note On, channel 3, key 69, velocity 90).
  auto noteMsg = bmmidi::NoteMsg::on(
      bmmidi::Channel::index(2), bmmidi::KeyNumber::key(69), bmmidi::DataValue{90});

  EXPECT_THAT(noteMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(noteMsg.isNoteOff(), IsFalse());
  EXPECT_THAT(noteMsg.isNoteOn(), IsTrue());
  EXPECT_THAT(noteMsg.key().value(), Eq(69));
  EXPECT_THAT(noteMsg.velocity().value(), Eq(90));

  // Can then mutate this into a note off.
  noteMsg.setType(bmmidi::MsgType::kNoteOff);
  noteMsg.setChannel(bmmidi::Channel::index(13));
  noteMsg.setKey(bmmidi::KeyNumber::key(60));
  noteMsg.setVelocity(bmmidi::DataValue{22});

  EXPECT_THAT(noteMsg.channel().displayNumber(), Eq(14));
  EXPECT_THAT(noteMsg.isNoteOff(), IsTrue());
  EXPECT_THAT(noteMsg.isNoteOn(), IsFalse());
  EXPECT_THAT(noteMsg.key().value(), Eq(60));
  EXPECT_THAT(noteMsg.velocity().value(), Eq(22));
}

TEST(NoteMsg, ShouldCreateNoteOffDefaultVelocity) {
  // If no 3rd argument is given, should default to velocity 0.
  // (Note Off, channel 3, key 69, velocity 0).
  auto noteMsg =
      bmmidi::NoteMsg::off(bmmidi::Channel::index(2), bmmidi::KeyNumber::key(69));

  EXPECT_THAT(noteMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(noteMsg.isNoteOff(), IsTrue());
  EXPECT_THAT(noteMsg.isNoteOn(), IsFalse());
  EXPECT_THAT(noteMsg.key().value(), Eq(69));
  EXPECT_THAT(noteMsg.velocity().value(), Eq(0));

  // Can then mutate this into a note on.
  noteMsg.setType(bmmidi::MsgType::kNoteOn);
  noteMsg.setChannel(bmmidi::Channel::index(13));
  noteMsg.setKey(bmmidi::KeyNumber::key(60));
  noteMsg.setVelocity(bmmidi::DataValue{22});

  EXPECT_THAT(noteMsg.channel().displayNumber(), Eq(14));
  EXPECT_THAT(noteMsg.isNoteOff(), IsFalse());
  EXPECT_THAT(noteMsg.isNoteOn(), IsTrue());
  EXPECT_THAT(noteMsg.key().value(), Eq(60));
  EXPECT_THAT(noteMsg.velocity().value(), Eq(22));
}

TEST(NoteMsg, ShouldCreateNoteOffSpecificVelocity) {
  // Can provide a 3rd argument for note off velocity.
  // (Note Off, channel 3, key 69, velocity 88).
  auto noteMsg = bmmidi::NoteMsg::off(
      bmmidi::Channel::index(2), bmmidi::KeyNumber::key(69), bmmidi::DataValue{88});

  EXPECT_THAT(noteMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(noteMsg.isNoteOff(), IsTrue());
  EXPECT_THAT(noteMsg.isNoteOn(), IsFalse());
  EXPECT_THAT(noteMsg.key().value(), Eq(69));
  EXPECT_THAT(noteMsg.velocity().value(), Eq(88));
}

TEST(NoteMsg, ShouldConvertFromMsg) {
  // A more generic Msg...
  // (Note On, channel 3, key 69, velocity 90).
  bmmidi::Msg<3> msg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                   bmmidi::Channel::index(2)),
      bmmidi::DataValue{69},
      bmmidi::DataValue{90}};
  
  // ...should convert to the more specific NoteMsg...
  auto noteMsg = msg.to<bmmidi::NoteMsg>();

  EXPECT_THAT(noteMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(noteMsg.isNoteOff(), IsFalse());
  EXPECT_THAT(noteMsg.isNoteOn(), IsTrue());
  EXPECT_THAT(noteMsg.key().value(), Eq(69));
  EXPECT_THAT(noteMsg.velocity().value(), Eq(90));

  // ...and this converted copy should be mutable...
  noteMsg.setVelocity(bmmidi::DataValue{23});
  EXPECT_THAT(noteMsg.velocity().value(), Eq(23));

  // ...and changes to it should NOT affect original Msg.
  EXPECT_THAT(msg.data2().value(), Eq(90));
}

TEST(KeyPressureMsg, ShouldCreate) {
  // (Polyphonic Key Pressure, channel 3, key 69, pressure 90).
  auto kpMsg = bmmidi::KeyPressureMsg{
      bmmidi::Channel::index(2), bmmidi::KeyNumber::key(69), bmmidi::DataValue{90}};

  EXPECT_THAT(kpMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(kpMsg.key().value(), Eq(69));
  EXPECT_THAT(kpMsg.pressure().value(), Eq(90));

  // Can mutate:
  kpMsg.setChannel(bmmidi::Channel::index(13));
  kpMsg.setKey(bmmidi::KeyNumber::key(60));
  kpMsg.setPressure(bmmidi::DataValue{22});

  EXPECT_THAT(kpMsg.channel().displayNumber(), Eq(14));
  EXPECT_THAT(kpMsg.key().value(), Eq(60));
  EXPECT_THAT(kpMsg.pressure().value(), Eq(22));
}

TEST(KeyPressureMsg, ShouldConvertFromMsg) {
  // A more generic Msg...
  // (Polyphonic Key Pressure, channel 3, key 69, pressure 90).
  bmmidi::Msg<3> msg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kPolyphonicKeyPressure,
                                   bmmidi::Channel::index(2)),
      bmmidi::DataValue{69},
      bmmidi::DataValue{90}};
  
  // ...should convert to the more specific KeyPressureMsg...
  auto kpMsg = msg.to<bmmidi::KeyPressureMsg>();

  EXPECT_THAT(kpMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(kpMsg.key().value(), Eq(69));
  EXPECT_THAT(kpMsg.pressure().value(), Eq(90));

  // ...and this converted copy should be mutable...
  kpMsg.setPressure(bmmidi::DataValue{23});
  EXPECT_THAT(kpMsg.pressure().value(), Eq(23));

  // ...and changes to it should NOT affect original Msg.
  EXPECT_THAT(msg.data2().value(), Eq(90));
}

TEST(ControlChangeMsg, ShouldCreate) {
  // (Control Change, channel 3, CC #11 expression, value 90).
  auto ccMsg = bmmidi::ControlChangeMsg{
     bmmidi::Channel::index(2), bmmidi::Control::kExpression, bmmidi::DataValue{90}};

  EXPECT_THAT(ccMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(ccMsg.control(), Eq(bmmidi::Control::kExpression));
  EXPECT_THAT(ccMsg.value(), Eq(bmmidi::DataValue{90}));

  // Can mutate:
  ccMsg.setChannel(bmmidi::Channel::index(13));
  ccMsg.setControl(bmmidi::Control::kModWheel);
  ccMsg.setValue(bmmidi::DataValue{22});

  EXPECT_THAT(ccMsg.channel().displayNumber(), Eq(14));
  EXPECT_THAT(ccMsg.control(), Eq(bmmidi::Control::kModWheel));
  EXPECT_THAT(ccMsg.value(), Eq(bmmidi::DataValue{22}));
}

TEST(ControlChangeMsg, ShouldConvertFromMsg) {
  // A more generic Msg...
  // (Control Change, channel 3, CC #11 expression, value 90).
  bmmidi::Msg<3> msg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(2)),
      bmmidi::DataValue{11},
      bmmidi::DataValue{90}};
  
  // ...should convert to the more specific ControlChangeMsg...
  auto ccMsg = msg.to<bmmidi::ControlChangeMsg>();

  EXPECT_THAT(ccMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(ccMsg.control(), Eq(bmmidi::Control::kExpression));
  EXPECT_THAT(ccMsg.value(), Eq(bmmidi::DataValue{90}));

  // ...and this converted copy should be mutable...
  ccMsg.setControl(bmmidi::Control::kModWheel);
  EXPECT_THAT(ccMsg.control(), Eq(bmmidi::Control::kModWheel));

  // ...and changes to it should NOT affect original Msg.
  EXPECT_THAT(msg.data1().value(), Eq(11));
}

TEST(ProgramChangeMsg, ShouldCreate) {
  // (Program Change, channel 3, program 57).
  auto pcMsg = bmmidi::ProgramChangeMsg{
     bmmidi::Channel::index(2), bmmidi::PresetNumber::index(56)};

  EXPECT_THAT(pcMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(pcMsg.program().displayNumber(), Eq(57));

  // Can mutate:
  pcMsg.setChannel(bmmidi::Channel::index(13));
  pcMsg.setProgram(bmmidi::PresetNumber::index(22));

  EXPECT_THAT(pcMsg.channel().displayNumber(), Eq(14));
  EXPECT_THAT(pcMsg.program().displayNumber(), Eq(23));
}

TEST(ProgramChangeMsg, ShouldConvertFromMsg) {
  // A more generic Msg...
  // (Program Change, channel 3, program 57).
  bmmidi::Msg<2> msg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                   bmmidi::Channel::index(2)),
      bmmidi::DataValue{56}};
  
  // ...should convert to the more specific ProgramChangeMsg...
  auto pcMsg = msg.to<bmmidi::ProgramChangeMsg>();

  EXPECT_THAT(pcMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(pcMsg.program().displayNumber(), Eq(57));

  // ...and this converted copy should be mutable...
  pcMsg.setProgram(bmmidi::PresetNumber::index(22));
  EXPECT_THAT(pcMsg.program().displayNumber(), Eq(23));

  // ...and changes to it should NOT affect original Msg.
  EXPECT_THAT(msg.data1().value(), Eq(56));
}

}  // namespace
