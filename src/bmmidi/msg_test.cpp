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
  bmmidi::Msg<1> oscTuneRequest{
      bmmidi::Status::system(bmmidi::MsgType::kOscillatorTuneRequest)};

  EXPECT_THAT(oscTuneRequest.type(), Eq(bmmidi::MsgType::kOscillatorTuneRequest));
  EXPECT_THAT(oscTuneRequest.status(),
      Eq(bmmidi::Status::system(bmmidi::MsgType::kOscillatorTuneRequest)));

  // The following functions should NOT exist, so they should cause compilation
  // errors if these lines are uncommented:
  //     oscTuneRequest.data1();
  //     oscTuneRequest.data2();
}

TEST(Msg, TwoByteVersionHasData1) {
  bmmidi::Msg<2> programChangeCh13{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                   bmmidi::Channel::index(12)),
      bmmidi::DataValue{57}};

  EXPECT_THAT(programChangeCh13.type(), Eq(bmmidi::MsgType::kProgramChange));
  EXPECT_THAT(programChangeCh13.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                      bmmidi::Channel::index(12))));
  EXPECT_THAT(programChangeCh13.data1(), Eq(bmmidi::DataValue{57}));

  // The following functions should NOT exist, so they should cause compilation
  // errors if these lines are uncommented:
  //     programChangeCh13.data2();
}

TEST(Msg, ThreeByteVersionHasData2) {
  bmmidi::Msg<3> controlChangeCh10{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(9)),
      bmmidi::controlToDataValue(bmmidi::Control::kExpression),
      bmmidi::DataValue{76}};

  EXPECT_THAT(controlChangeCh10.type(), Eq(bmmidi::MsgType::kControlChange));
  EXPECT_THAT(controlChangeCh10.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(9))));
  EXPECT_THAT(controlChangeCh10.data1(), Eq(bmmidi::DataValue{11}));
  EXPECT_THAT(controlChangeCh10.data2(), Eq(bmmidi::DataValue{76}));
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
  bmmidi::Msg<1> oscTuneRequest{
      bmmidi::Status::system(bmmidi::MsgType::kOscillatorTuneRequest)};
  EXPECT_THAT(sizeof(oscTuneRequest.rawBytes()), Eq(1));
  EXPECT_THAT(oscTuneRequest.rawBytes()[0], Eq(std::uint8_t{0xF6}));

  // Two byte version:
  bmmidi::Msg<2> programChangeCh13{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                   bmmidi::Channel::index(12)),
      bmmidi::DataValue{57}};
  EXPECT_THAT(sizeof(programChangeCh13.rawBytes()), Eq(2));
  EXPECT_THAT(programChangeCh13.rawBytes()[0], Eq(std::uint8_t{0xCC}));
  EXPECT_THAT(programChangeCh13.rawBytes()[1], Eq(std::uint8_t{57}));

  // Three byte version:
  bmmidi::Msg<3> controlChangeCh10{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(9)),
      bmmidi::controlToDataValue(bmmidi::Control::kExpression),
      bmmidi::DataValue{76}};
  EXPECT_THAT(sizeof(controlChangeCh10.rawBytes()), Eq(3));
  EXPECT_THAT(controlChangeCh10.rawBytes()[0], Eq(std::uint8_t{0xB9}));
  EXPECT_THAT(controlChangeCh10.rawBytes()[1], Eq(std::uint8_t{11}));
  EXPECT_THAT(controlChangeCh10.rawBytes()[2], Eq(std::uint8_t{76}));
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
  auto view = ccCh13Mod37.view();

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
  auto ref = ccCh13Mod37.ref();

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

}  // namespace
