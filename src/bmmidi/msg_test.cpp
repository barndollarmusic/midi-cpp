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

TEST(Msg, ShouldBePackedSize) {
  EXPECT_THAT(sizeof(bmmidi::Msg<1>), Eq(1));
  EXPECT_THAT(sizeof(bmmidi::Msg<2>), Eq(2));
  EXPECT_THAT(sizeof(bmmidi::Msg<3>), Eq(3));
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

TEST(NoteMsgView, ShouldYieldCorrectData) {
  {
    // Note Off, channel 13, key 61, velocity 0.
    const std::uint8_t srcBytes[] = {0x8C, 0x3D, 0x00};
    bmmidi::NoteMsgView view{srcBytes, sizeof(srcBytes)};

    EXPECT_THAT(view.channel().displayNumber(), Eq(13));
    EXPECT_THAT(view.isNoteOff(), IsTrue());
    EXPECT_THAT(view.isNoteOn(), IsFalse());
    EXPECT_THAT(view.key().value(), Eq(61));
    EXPECT_THAT(view.velocity().value(), Eq(0));
  }

  {
    // Note Off, channel 1, key 127, velocity 93.
    const auto msg = bmmidi::NoteMsg::off(bmmidi::Channel::index(0),
                                          bmmidi::KeyNumber::key(127),
                                          bmmidi::DataValue{93});
    auto view = msg.asView<bmmidi::NoteMsgView>();

    EXPECT_THAT(view.channel().displayNumber(), Eq(1));
    EXPECT_THAT(view.isNoteOff(), IsTrue());
    EXPECT_THAT(view.isNoteOn(), IsFalse());
    EXPECT_THAT(view.key().value(), Eq(127));
    EXPECT_THAT(view.velocity().value(), Eq(93));
  }

  {
    // Note On, channel 3, key 69, velocity 0 (should treat as Note Off).
    const std::uint8_t srcBytes[] = {0x92, 0x45, 0x00};
    bmmidi::NoteMsgView view{srcBytes, sizeof(srcBytes)};

    EXPECT_THAT(view.channel().displayNumber(), Eq(3));
    EXPECT_THAT(view.isNoteOff(), IsTrue());
    EXPECT_THAT(view.isNoteOn(), IsFalse());
    EXPECT_THAT(view.key().value(), Eq(69));
    EXPECT_THAT(view.velocity().value(), Eq(0));
  }

  {
    // Note On, channel 3, key 69, velocity 90.
    const auto msg = bmmidi::NoteMsg::on(bmmidi::Channel::index(2),
                                         bmmidi::KeyNumber::key(69),
                                         bmmidi::DataValue{90});
    auto view = msg.asView<bmmidi::NoteMsgView>();

    EXPECT_THAT(view.channel().displayNumber(), Eq(3));
    EXPECT_THAT(view.isNoteOff(), IsFalse());
    EXPECT_THAT(view.isNoteOn(), IsTrue());
    EXPECT_THAT(view.key().value(), Eq(69));
    EXPECT_THAT(view.velocity().value(), Eq(90));
  }
}

// TODO: Add more thorough tests for:
//   - ChanMsgView, ChanMsgRef
//   - NoteMsgView, NoteMsgRef
//   - ChanMsg<2>, ChanMsg<3>
//   - NoteMsg

// TODO: Add tests for MsgView and MsgRef referring to SysExMsg.

}  // namespace
