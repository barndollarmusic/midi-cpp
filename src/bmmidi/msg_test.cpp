#include "bmmidi/msg_reference.hpp"
#include "bmmidi/msg.hpp"

#include <cstdint>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bmmidi/channel.hpp"
#include "bmmidi/control.hpp"
#include "bmmidi/data_value.hpp"
#include "bmmidi/msg_reference.hpp"
#include "bmmidi/status.hpp"
#include "bmmidi/timecode.hpp"

namespace {

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;

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

  EXPECT_THAT(msg.rawBytes()[0], Eq(0xF6));

  msg.setStatus(bmmidi::Status::system(bmmidi::MsgType::kSystemReset));
  EXPECT_THAT(msg.type(), Eq(bmmidi::MsgType::kSystemReset));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::system(bmmidi::MsgType::kSystemReset)));

  EXPECT_THAT(msg.rawBytes()[0], Eq(0xFF));
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

  EXPECT_THAT(msg.rawBytes()[0], Eq(0xCC));
  EXPECT_THAT(msg.rawBytes()[1], Eq(0x39));

  msg.setData1(bmmidi::DataValue{63});
  EXPECT_THAT(msg.type(), Eq(bmmidi::MsgType::kProgramChange));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                      bmmidi::Channel::index(12))));
  EXPECT_THAT(msg.data1(), Eq(bmmidi::DataValue{63}));

  EXPECT_THAT(msg.rawBytes()[0], Eq(0xCC));
  EXPECT_THAT(msg.rawBytes()[1], Eq(0x3F));

  msg.setStatus(
      bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                   bmmidi::Channel::index(3)));
  EXPECT_THAT(msg.type(), Eq(bmmidi::MsgType::kChannelPressure));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                      bmmidi::Channel::index(3))));
  EXPECT_THAT(msg.data1(), Eq(bmmidi::DataValue{63}));

  EXPECT_THAT(msg.rawBytes()[0], Eq(0xD3));
  EXPECT_THAT(msg.rawBytes()[1], Eq(0x3F));
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

  EXPECT_THAT(msg.rawBytes()[0], Eq(0xB9));
  EXPECT_THAT(msg.rawBytes()[1], Eq(0x0B));
  EXPECT_THAT(msg.rawBytes()[2], Eq(0x4C));

  msg.setData2(bmmidi::DataValue{27});
  EXPECT_THAT(msg.type(), Eq(bmmidi::MsgType::kControlChange));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(9))));
  EXPECT_THAT(msg.data1(), Eq(bmmidi::DataValue{11}));
  EXPECT_THAT(msg.data2(), Eq(bmmidi::DataValue{27}));

  EXPECT_THAT(msg.rawBytes()[0], Eq(0xB9));
  EXPECT_THAT(msg.rawBytes()[1], Eq(0x0B));
  EXPECT_THAT(msg.rawBytes()[2], Eq(0x1B));

  msg.setData1(bmmidi::controlToDataValue(bmmidi::Control::kModWheel));
  EXPECT_THAT(msg.type(), Eq(bmmidi::MsgType::kControlChange));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                      bmmidi::Channel::index(9))));
  EXPECT_THAT(msg.data1(), Eq(bmmidi::DataValue{1}));
  EXPECT_THAT(msg.data2(), Eq(bmmidi::DataValue{27}));

  EXPECT_THAT(msg.rawBytes()[0], Eq(0xB9));
  EXPECT_THAT(msg.rawBytes()[1], Eq(0x01));
  EXPECT_THAT(msg.rawBytes()[2], Eq(0x1B));

  msg.setStatus(
      bmmidi::Status::channelVoice(bmmidi::MsgType::kPolyphonicKeyPressure,
                                   bmmidi::Channel::index(3)));
  EXPECT_THAT(msg.type(), Eq(bmmidi::MsgType::kPolyphonicKeyPressure));
  EXPECT_THAT(msg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kPolyphonicKeyPressure,
                                      bmmidi::Channel::index(3))));
  EXPECT_THAT(msg.data1(), Eq(bmmidi::DataValue{1}));
  EXPECT_THAT(msg.data2(), Eq(bmmidi::DataValue{27}));

  EXPECT_THAT(msg.rawBytes()[0], Eq(0xA3));
  EXPECT_THAT(msg.rawBytes()[1], Eq(0x01));
  EXPECT_THAT(msg.rawBytes()[2], Eq(0x1B));
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

TEST(Msg, ConvertsToMsgView) {
  {
    // Supports explicit conversion with asView():
    bmmidi::Msg<3> msg{
        bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                     bmmidi::Channel::index(9)),
        bmmidi::controlToDataValue(bmmidi::Control::kExpression),
        bmmidi::DataValue{76}};

    auto view = msg.asView<bmmidi::MsgView>();  // Explicit.
    EXPECT_THAT(view.type(), Eq(bmmidi::MsgType::kControlChange));
    EXPECT_THAT(view.status().channel().index(), Eq(9));
    EXPECT_THAT(view.data1().value(), Eq(11));
    EXPECT_THAT(view.data2().value(), Eq(76));

    // If msg is changed, the view will reflect that.
    msg.setData2(bmmidi::DataValue{88});
    EXPECT_THAT(view.data2().value(), Eq(88));
  }

  {
    // Supports implicit conversion through operator MsgView():
    bmmidi::Msg<3> msg{
        bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                     bmmidi::Channel::index(9)),
        bmmidi::controlToDataValue(bmmidi::Control::kExpression),
        bmmidi::DataValue{76}};

    bmmidi::MsgView view = msg;  // Implicit.
    EXPECT_THAT(view.type(), Eq(bmmidi::MsgType::kControlChange));
    EXPECT_THAT(view.status().channel().index(), Eq(9));
    EXPECT_THAT(view.data1().value(), Eq(11));
    EXPECT_THAT(view.data2().value(), Eq(76));

    // If msg is changed, the view will reflect that.
    msg.setData2(bmmidi::DataValue{88});
    EXPECT_THAT(view.data2().value(), Eq(88));
  }
}

TEST(Msg, ConvertsToMsgRef) {
  {
    // Supports explicit conversion with asRef():
    bmmidi::Msg<3> msg{
        bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                     bmmidi::Channel::index(9)),
        bmmidi::controlToDataValue(bmmidi::Control::kExpression),
        bmmidi::DataValue{76}};

    auto ref = msg.asRef<bmmidi::MsgRef>();  // Explicit.
    EXPECT_THAT(ref.type(), Eq(bmmidi::MsgType::kControlChange));
    EXPECT_THAT(ref.status().channel().index(), Eq(9));
    EXPECT_THAT(ref.data1().value(), Eq(11));
    EXPECT_THAT(ref.data2().value(), Eq(76));

    // If msg is changed, the ref will reflect that.
    msg.setData2(bmmidi::DataValue{88});
    EXPECT_THAT(ref.data2().value(), Eq(88));

    // And can also change the msg through ref.
    ref.setData1(bmmidi::DataValue{1});
    EXPECT_THAT(msg.data1().value(), Eq(1));
  }

  {
    // Supports implicit conversion through operator MsgRef():
    bmmidi::Msg<3> msg{
        bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                     bmmidi::Channel::index(9)),
        bmmidi::controlToDataValue(bmmidi::Control::kExpression),
        bmmidi::DataValue{76}};

    bmmidi::MsgRef ref = msg;  // Implicit.
    EXPECT_THAT(ref.type(), Eq(bmmidi::MsgType::kControlChange));
    EXPECT_THAT(ref.status().channel().index(), Eq(9));
    EXPECT_THAT(ref.data1().value(), Eq(11));
    EXPECT_THAT(ref.data2().value(), Eq(76));

    // If msg is changed, the ref will reflect that.
    msg.setData2(bmmidi::DataValue{88});
    EXPECT_THAT(ref.data2().value(), Eq(88));

    // And can also change the msg through ref.
    ref.setData1(bmmidi::DataValue{1});
    EXPECT_THAT(msg.data1().value(), Eq(1));
  }
}

TEST(TimedMsg, ConvertsToTimedMsgView) {
  {
    // Supports explicit conversion with asView():
    auto msg = bmmidi::TimedMsg<bmmidi::Msg<3>>{1.01,
        bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                     bmmidi::Channel::index(2)),
        bmmidi::DataValue{69},
        bmmidi::DataValue{90}};

    auto view = msg.asView<bmmidi::MsgView>();  // Explicit.
    EXPECT_THAT(view.value().type(), Eq(bmmidi::MsgType::kNoteOn));
    EXPECT_THAT(view.value().status().channel().index(), Eq(2));
    EXPECT_THAT(view.value().data1().value(), Eq(69));
    EXPECT_THAT(view.value().data2().value(), Eq(90));

    // If msg is changed (other than timestamp), the view will reflect that.
    msg.value().setData2(bmmidi::DataValue{88});
    EXPECT_THAT(view.value().data2().value(), Eq(88));
  }

  {
    // Supports implicit conversion through operator TimedMsgView():
    auto msg = bmmidi::TimedMsg<bmmidi::Msg<3>>{1.01,
        bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                     bmmidi::Channel::index(2)),
        bmmidi::DataValue{69},
        bmmidi::DataValue{90}};

    bmmidi::TimedMsgView view = msg;  // Implicit.
    EXPECT_THAT(view.value().type(), Eq(bmmidi::MsgType::kNoteOn));
    EXPECT_THAT(view.value().status().channel().index(), Eq(2));
    EXPECT_THAT(view.value().data1().value(), Eq(69));
    EXPECT_THAT(view.value().data2().value(), Eq(90));

    // If msg is changed (other than timestamp), the view will reflect that.
    msg.value().setData2(bmmidi::DataValue{88});
    EXPECT_THAT(view.value().data2().value(), Eq(88));
  }
}

TEST(TimedMsg, ConvertsToTimedMsgRef) {
  {
    // Supports explicit conversion with asRef():
    auto msg = bmmidi::TimedMsg<bmmidi::Msg<3>>{1.01,
        bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                     bmmidi::Channel::index(2)),
        bmmidi::DataValue{69},
        bmmidi::DataValue{90}};

    auto ref = msg.asRef<bmmidi::MsgRef>();  // Explicit.
    EXPECT_THAT(ref.value().type(), Eq(bmmidi::MsgType::kNoteOn));
    EXPECT_THAT(ref.value().status().channel().index(), Eq(2));
    EXPECT_THAT(ref.value().data1().value(), Eq(69));
    EXPECT_THAT(ref.value().data2().value(), Eq(90));

    // If msg is changed (other than timestamp), the ref will reflect that.
    msg.value().setData2(bmmidi::DataValue{88});
    EXPECT_THAT(ref.value().data2().value(), Eq(88));

    // And changing ref will also affect msg.
    ref.value().setData1(bmmidi::DataValue{60});
    EXPECT_THAT(msg.value().data1().value(), Eq(60));
  }

  {
    // Supports implicit conversion through operator TimedMsgRef():
    auto msg = bmmidi::TimedMsg<bmmidi::Msg<3>>{1.01,
        bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                     bmmidi::Channel::index(2)),
        bmmidi::DataValue{69},
        bmmidi::DataValue{90}};

    bmmidi::TimedMsgRef ref = msg;  // Implicit.
    EXPECT_THAT(ref.value().type(), Eq(bmmidi::MsgType::kNoteOn));
    EXPECT_THAT(ref.value().status().channel().index(), Eq(2));
    EXPECT_THAT(ref.value().data1().value(), Eq(69));
    EXPECT_THAT(ref.value().data2().value(), Eq(90));

    // If msg is changed (other than timestamp), the ref will reflect that.
    msg.value().setData2(bmmidi::DataValue{88});
    EXPECT_THAT(ref.value().data2().value(), Eq(88));

    // And changing ref will also affect msg.
    ref.value().setData1(bmmidi::DataValue{60});
    EXPECT_THAT(msg.value().data1().value(), Eq(60));
  }
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

  EXPECT_THAT(chanMsg.rawBytes()[0], Eq(0xCC));
  EXPECT_THAT(chanMsg.rawBytes()[1], Eq(0x39));

  // Can mutate channel:
  chanMsg.setChannel(bmmidi::Channel::index(2));
  
  EXPECT_THAT(chanMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(chanMsg.type(), Eq(bmmidi::MsgType::kProgramChange));
  EXPECT_THAT(chanMsg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                      bmmidi::Channel::index(2))));
  EXPECT_THAT(chanMsg.data1(), Eq(bmmidi::DataValue{57}));

  EXPECT_THAT(chanMsg.rawBytes()[0], Eq(0xC2));
  EXPECT_THAT(chanMsg.rawBytes()[1], Eq(0x39));
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

  EXPECT_THAT(chanMsg.rawBytes()[0], Eq(0x92));
  EXPECT_THAT(chanMsg.rawBytes()[1], Eq(0x45));
  EXPECT_THAT(chanMsg.rawBytes()[2], Eq(0x5A));

  // Can mutate channel:
  chanMsg.setChannel(bmmidi::Channel::index(13));
  
  EXPECT_THAT(chanMsg.channel().displayNumber(), Eq(14));
  EXPECT_THAT(chanMsg.type(), Eq(bmmidi::MsgType::kNoteOn));
  EXPECT_THAT(chanMsg.status(),
      Eq(bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                      bmmidi::Channel::index(13))));
  EXPECT_THAT(chanMsg.data1(), Eq(bmmidi::DataValue{69}));
  EXPECT_THAT(chanMsg.data2(), Eq(bmmidi::DataValue{90}));

  EXPECT_THAT(chanMsg.rawBytes()[0], Eq(0x9D));
  EXPECT_THAT(chanMsg.rawBytes()[1], Eq(0x45));
  EXPECT_THAT(chanMsg.rawBytes()[2], Eq(0x5A));
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

TEST(ChanMsg, ConvertsToChanMsgView) {
  {
    // Supports explicit conversion with asView():
    bmmidi::ChanMsg<3> msg{
        bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                     bmmidi::Channel::index(2)),
        bmmidi::DataValue{69},
        bmmidi::DataValue{90}};

    auto view = msg.asView<bmmidi::ChanMsgView>();  // Explicit.
    EXPECT_THAT(view.type(), Eq(bmmidi::MsgType::kNoteOn));
    EXPECT_THAT(view.status().channel().index(), Eq(2));
    EXPECT_THAT(view.data1().value(), Eq(69));
    EXPECT_THAT(view.data2().value(), Eq(90));

    // If msg is changed, the view will reflect that.
    msg.setChannel(bmmidi::Channel::index(7));
    EXPECT_THAT(view.status().channel().index(), Eq(7));
  }

  {
    // Supports implicit conversion through operator ChanMsgView():
    bmmidi::ChanMsg<3> msg{
        bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                     bmmidi::Channel::index(2)),
        bmmidi::DataValue{69},
        bmmidi::DataValue{90}};

    bmmidi::ChanMsgView view = msg;  // Implicit.
    EXPECT_THAT(view.type(), Eq(bmmidi::MsgType::kNoteOn));
    EXPECT_THAT(view.status().channel().index(), Eq(2));
    EXPECT_THAT(view.data1().value(), Eq(69));
    EXPECT_THAT(view.data2().value(), Eq(90));

    // If msg is changed, the view will reflect that.
    msg.setChannel(bmmidi::Channel::index(7));
    EXPECT_THAT(view.status().channel().index(), Eq(7));
  }
}

TEST(ChanMsg, ConvertsToChanMsgRef) {
  {
    // Supports explicit conversion with asRef():
    bmmidi::ChanMsg<3> msg{
        bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                     bmmidi::Channel::index(2)),
        bmmidi::DataValue{69},
        bmmidi::DataValue{90}};

    auto ref = msg.asRef<bmmidi::ChanMsgRef>();  // Explicit.
    EXPECT_THAT(ref.type(), Eq(bmmidi::MsgType::kNoteOn));
    EXPECT_THAT(ref.status().channel().index(), Eq(2));
    EXPECT_THAT(ref.data1().value(), Eq(69));
    EXPECT_THAT(ref.data2().value(), Eq(90));

    // If msg is changed, the ref will reflect that.
    msg.setChannel(bmmidi::Channel::index(7));
    EXPECT_THAT(ref.status().channel().index(), Eq(7));

    // And changing ref will also change msg.
    ref.setChannel(bmmidi::Channel::index(12));
    EXPECT_THAT(msg.channel().index(), Eq(12));
  }

  {
    // Supports implicit conversion through operator ChanMsgRef():
    bmmidi::ChanMsg<3> msg{
        bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                     bmmidi::Channel::index(2)),
        bmmidi::DataValue{69},
        bmmidi::DataValue{90}};

    bmmidi::ChanMsgRef ref = msg;  // Implicit.
    EXPECT_THAT(ref.type(), Eq(bmmidi::MsgType::kNoteOn));
    EXPECT_THAT(ref.status().channel().index(), Eq(2));
    EXPECT_THAT(ref.data1().value(), Eq(69));
    EXPECT_THAT(ref.data2().value(), Eq(90));

    // If msg is changed, the ref will reflect that.
    msg.setChannel(bmmidi::Channel::index(7));
    EXPECT_THAT(ref.status().channel().index(), Eq(7));

    // And changing ref will also change msg.
    ref.setChannel(bmmidi::Channel::index(12));
    EXPECT_THAT(msg.channel().index(), Eq(12));
  }
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

  EXPECT_THAT(noteMsg.rawBytes()[0], Eq(0x92));
  EXPECT_THAT(noteMsg.rawBytes()[1], Eq(0x45));
  EXPECT_THAT(noteMsg.rawBytes()[2], Eq(0x5A));

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

  EXPECT_THAT(noteMsg.rawBytes()[0], Eq(0x8D));
  EXPECT_THAT(noteMsg.rawBytes()[1], Eq(0x3C));
  EXPECT_THAT(noteMsg.rawBytes()[2], Eq(0x16));
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

  EXPECT_THAT(noteMsg.rawBytes()[0], Eq(0x82));
  EXPECT_THAT(noteMsg.rawBytes()[1], Eq(0x45));
  EXPECT_THAT(noteMsg.rawBytes()[2], Eq(0x00));

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

  EXPECT_THAT(noteMsg.rawBytes()[0], Eq(0x9D));
  EXPECT_THAT(noteMsg.rawBytes()[1], Eq(0x3C));
  EXPECT_THAT(noteMsg.rawBytes()[2], Eq(0x16));
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

  EXPECT_THAT(noteMsg.rawBytes()[0], Eq(0x82));
  EXPECT_THAT(noteMsg.rawBytes()[1], Eq(0x45));
  EXPECT_THAT(noteMsg.rawBytes()[2], Eq(0x58));
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

TEST(NoteMsg, ConvertsToNoteMsgView) {
  {
    // Supports explicit conversion with asView():
    auto msg = bmmidi::NoteMsg::on(
        bmmidi::Channel::index(2), bmmidi::KeyNumber::key(69), bmmidi::DataValue{90});

    auto view = msg.asView<bmmidi::NoteMsgView>();  // Explicit.
    EXPECT_THAT(view.type(), Eq(bmmidi::MsgType::kNoteOn));
    EXPECT_THAT(view.status().channel().index(), Eq(2));
    EXPECT_THAT(view.data1().value(), Eq(69));
    EXPECT_THAT(view.data2().value(), Eq(90));

    // If msg is changed, the view will reflect that.
    msg.setVelocity(bmmidi::DataValue{32});
    EXPECT_THAT(view.data2().value(), Eq(32));
  }

  {
    // Supports implicit conversion through operator NoteMsgView():
    auto msg = bmmidi::NoteMsg::on(
        bmmidi::Channel::index(2), bmmidi::KeyNumber::key(69), bmmidi::DataValue{90});

    bmmidi::NoteMsgView view = msg;  // Implicit.
    EXPECT_THAT(view.type(), Eq(bmmidi::MsgType::kNoteOn));
    EXPECT_THAT(view.status().channel().index(), Eq(2));
    EXPECT_THAT(view.data1().value(), Eq(69));
    EXPECT_THAT(view.data2().value(), Eq(90));

    // If msg is changed, the view will reflect that.
    msg.setVelocity(bmmidi::DataValue{32});
    EXPECT_THAT(view.data2().value(), Eq(32));
  }
}

TEST(NoteMsg, ConvertsToNoteMsgRef) {
  {
    // Supports explicit conversion with asRef():
    auto msg = bmmidi::NoteMsg::on(
        bmmidi::Channel::index(2), bmmidi::KeyNumber::key(69), bmmidi::DataValue{90});

    auto ref = msg.asRef<bmmidi::NoteMsgRef>();  // Explicit.
    EXPECT_THAT(ref.type(), Eq(bmmidi::MsgType::kNoteOn));
    EXPECT_THAT(ref.status().channel().index(), Eq(2));
    EXPECT_THAT(ref.data1().value(), Eq(69));
    EXPECT_THAT(ref.data2().value(), Eq(90));

    // If msg is changed, the ref will reflect that.
    msg.setVelocity(bmmidi::DataValue{32});
    EXPECT_THAT(ref.data2().value(), Eq(32));

    // And changing ref should also affect msg.
    ref.setKey(bmmidi::KeyNumber::key(60));
    EXPECT_THAT(msg.key().value(), Eq(60));
  }

  {
    // Supports implicit conversion through operator NoteMsgRef():
    auto msg = bmmidi::NoteMsg::on(
        bmmidi::Channel::index(2), bmmidi::KeyNumber::key(69), bmmidi::DataValue{90});

    bmmidi::NoteMsgRef ref = msg;  // Implicit.
    EXPECT_THAT(ref.type(), Eq(bmmidi::MsgType::kNoteOn));
    EXPECT_THAT(ref.status().channel().index(), Eq(2));
    EXPECT_THAT(ref.data1().value(), Eq(69));
    EXPECT_THAT(ref.data2().value(), Eq(90));

    // If msg is changed, the ref will reflect that.
    msg.setVelocity(bmmidi::DataValue{32});
    EXPECT_THAT(ref.data2().value(), Eq(32));

    // And changing ref should also affect msg.
    ref.setKey(bmmidi::KeyNumber::key(60));
    EXPECT_THAT(msg.key().value(), Eq(60));
  }
}

TEST(TimedNoteMsg, SupportsFactoriesAndConversion) {
  {
    // Time 1.01, Note On, Channel 4, Key 60, Velocity 88:
    auto msg = bmmidi::TimedNoteMsg::on(1.01,
        bmmidi::Channel::index(3), bmmidi::KeyNumber::key(60), bmmidi::DataValue{88});
    EXPECT_THAT(msg.timestamp(), Eq(1.01));
    EXPECT_THAT(msg.value().isNoteOn(), IsTrue());
    EXPECT_THAT(msg.value().channel().index(), Eq(3));
    EXPECT_THAT(msg.value().key().value(), Eq(60));
    EXPECT_THAT(msg.value().velocity().value(), Eq(88));
  }

  {
    // Time 2.02, Note Off, Channel 4, Key 60, Velocity 88:
    auto msg = bmmidi::TimedNoteMsg::off(2.02,
        bmmidi::Channel::index(3), bmmidi::KeyNumber::key(60), bmmidi::DataValue{88});
    EXPECT_THAT(msg.timestamp(), Eq(2.02));
    EXPECT_THAT(msg.value().isNoteOff(), IsTrue());
    EXPECT_THAT(msg.value().channel().index(), Eq(3));
    EXPECT_THAT(msg.value().key().value(), Eq(60));
    EXPECT_THAT(msg.value().velocity().value(), Eq(88));
  }

  {
    // Should default to note off velocity 0:
    auto msg = bmmidi::TimedNoteMsg::off(3.03,
        bmmidi::Channel::index(3), bmmidi::KeyNumber::key(60));
    EXPECT_THAT(msg.value().velocity().value(), Eq(0));
  }

  {
    // Should convert from TimedMsg<Msg<N>>:
    auto genericMsg = bmmidi::TimedMsg<bmmidi::Msg<3>>{4.04,
        bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                    bmmidi::Channel::index(2)),
        bmmidi::DataValue{69},
        bmmidi::DataValue{90}};

    bmmidi::TimedNoteMsg msg = genericMsg.to<bmmidi::NoteMsg>();
    EXPECT_THAT(msg.timestamp(), Eq(4.04));
    EXPECT_THAT(msg.value().isNoteOn(), IsTrue());
    EXPECT_THAT(msg.value().channel().index(), Eq(2));
    EXPECT_THAT(msg.value().key().value(), Eq(69));
    EXPECT_THAT(msg.value().velocity().value(), Eq(90));
  }
}

TEST(TimedNoteMsg, ConvertsToTimedNoteMsgView) {
  // Time 1.01, Note On, Channel 4, Key 60, Velocity 88:
  auto msg = bmmidi::TimedNoteMsg::on(1.01,
      bmmidi::Channel::index(3), bmmidi::KeyNumber::key(60), bmmidi::DataValue{88});
  const auto& constMsgRef = msg;

  bmmidi::TimedNoteMsgView view = constMsgRef;  // Implicit.
  EXPECT_THAT(view.timestamp(), Eq(1.01));
  EXPECT_THAT(view.value().isNoteOn(), IsTrue());
  EXPECT_THAT(view.value().channel().index(), Eq(3));
  EXPECT_THAT(view.value().key().value(), Eq(60));
  EXPECT_THAT(view.value().velocity().value(), Eq(88));

  // If msg is changed (except for timestamp), view should also reflect that.
  msg.value().setChannel(bmmidi::Channel::index(7));
  EXPECT_THAT(view.value().channel().index(), Eq(7));
}

TEST(TimedNoteMsg, ConvertsToTimedNoteMsgRef) {
  // Time 1.01, Note On, Channel 4, Key 60, Velocity 88:
  auto msg = bmmidi::TimedNoteMsg::on(1.01,
      bmmidi::Channel::index(3), bmmidi::KeyNumber::key(60), bmmidi::DataValue{88});

  bmmidi::TimedNoteMsgRef ref = msg;  // Implicit.
  EXPECT_THAT(ref.timestamp(), Eq(1.01));
  EXPECT_THAT(ref.value().isNoteOn(), IsTrue());
  EXPECT_THAT(ref.value().channel().index(), Eq(3));
  EXPECT_THAT(ref.value().key().value(), Eq(60));
  EXPECT_THAT(ref.value().velocity().value(), Eq(88));

  // If msg is changed (except for timestamp), ref should also reflect that.
  msg.value().setChannel(bmmidi::Channel::index(7));
  EXPECT_THAT(ref.value().channel().index(), Eq(7));

  // And if ref is changed, msg should also reflect that.
  ref.value().setType(bmmidi::MsgType::kNoteOff);
  EXPECT_THAT(msg.value().isNoteOff(), IsTrue());
}

TEST(KeyPressureMsg, ShouldCreate) {
  // (Polyphonic Key Pressure, channel 3, key 69, pressure 90).
  auto kpMsg = bmmidi::KeyPressureMsg{
      bmmidi::Channel::index(2), bmmidi::KeyNumber::key(69), bmmidi::DataValue{90}};

  EXPECT_THAT(kpMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(kpMsg.key().value(), Eq(69));
  EXPECT_THAT(kpMsg.pressure().value(), Eq(90));

  EXPECT_THAT(kpMsg.rawBytes()[0], Eq(0xA2));
  EXPECT_THAT(kpMsg.rawBytes()[1], Eq(0x45));
  EXPECT_THAT(kpMsg.rawBytes()[2], Eq(0x5A));

  // Can mutate:
  kpMsg.setChannel(bmmidi::Channel::index(13));
  kpMsg.setKey(bmmidi::KeyNumber::key(60));
  kpMsg.setPressure(bmmidi::DataValue{22});

  EXPECT_THAT(kpMsg.channel().displayNumber(), Eq(14));
  EXPECT_THAT(kpMsg.key().value(), Eq(60));
  EXPECT_THAT(kpMsg.pressure().value(), Eq(22));

  EXPECT_THAT(kpMsg.rawBytes()[0], Eq(0xAD));
  EXPECT_THAT(kpMsg.rawBytes()[1], Eq(0x3C));
  EXPECT_THAT(kpMsg.rawBytes()[2], Eq(0x16));
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

TEST(TimedKeyPressureMsg, SupportsFactoriesAndConversion) {
  {
    // Time 1.01, Polyphonic Key Pressure, Channel 3, Key 69, Pressure 90:
    auto msg = bmmidi::TimedKeyPressureMsg{1.01,
        bmmidi::Channel::index(2), bmmidi::KeyNumber::key(69), bmmidi::DataValue{90}};
    EXPECT_THAT(msg.timestamp(), Eq(1.01));
    EXPECT_THAT(msg.value().channel().index(), Eq(2));
    EXPECT_THAT(msg.value().key().value(), Eq(69));
    EXPECT_THAT(msg.value().pressure().value(), Eq(90));
  }

  {
    // Should convert from TimedMsg<Msg<N>>:
    auto genericMsg = bmmidi::TimedMsg<bmmidi::Msg<3>>{2.02,
        bmmidi::Status::channelVoice(bmmidi::MsgType::kPolyphonicKeyPressure,
                                     bmmidi::Channel::index(7)),
        bmmidi::DataValue{60},
        bmmidi::DataValue{88}};

    bmmidi::TimedKeyPressureMsg msg = genericMsg.to<bmmidi::KeyPressureMsg>();
    EXPECT_THAT(msg.timestamp(), Eq(2.02));
    EXPECT_THAT(msg.value().channel().index(), Eq(7));
    EXPECT_THAT(msg.value().key().value(), Eq(60));
    EXPECT_THAT(msg.value().pressure().value(), Eq(88));
  }
}

TEST(ControlChangeMsg, ShouldCreate) {
  // (Control Change, channel 3, CC #11 expression, value 90).
  auto ccMsg = bmmidi::ControlChangeMsg{
     bmmidi::Channel::index(2), bmmidi::Control::kExpression, bmmidi::DataValue{90}};

  EXPECT_THAT(ccMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(ccMsg.control(), Eq(bmmidi::Control::kExpression));
  EXPECT_THAT(ccMsg.value(), Eq(bmmidi::DataValue{90}));

  EXPECT_THAT(ccMsg.rawBytes()[0], Eq(0xB2));
  EXPECT_THAT(ccMsg.rawBytes()[1], Eq(0x0B));
  EXPECT_THAT(ccMsg.rawBytes()[2], Eq(0x5A));

  // Can mutate:
  ccMsg.setChannel(bmmidi::Channel::index(13));
  ccMsg.setControl(bmmidi::Control::kModWheel);
  ccMsg.setValue(bmmidi::DataValue{22});

  EXPECT_THAT(ccMsg.channel().displayNumber(), Eq(14));
  EXPECT_THAT(ccMsg.control(), Eq(bmmidi::Control::kModWheel));
  EXPECT_THAT(ccMsg.value(), Eq(bmmidi::DataValue{22}));

  EXPECT_THAT(ccMsg.rawBytes()[0], Eq(0xBD));
  EXPECT_THAT(ccMsg.rawBytes()[1], Eq(0x01));
  EXPECT_THAT(ccMsg.rawBytes()[2], Eq(0x16));
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

TEST(TimedControlChangeMsg, SupportsFactoriesAndConversion) {
  {
    // Time 1.01, Control Change, Channel 3, CC #11 Expression, Value 90.
    auto msg = bmmidi::TimedControlChangeMsg{1.01,
        bmmidi::Channel::index(2), bmmidi::Control::kExpression, bmmidi::DataValue{90}};
    EXPECT_THAT(msg.timestamp(), Eq(1.01));
    EXPECT_THAT(msg.value().channel().index(), Eq(2));
    EXPECT_THAT(msg.value().control(), Eq(bmmidi::Control::kExpression));
    EXPECT_THAT(msg.value().value(), Eq(bmmidi::DataValue{90}));
  }

  {
    // Should convert from TimedMsg<Msg<N>>:
    auto genericMsg = bmmidi::TimedMsg<bmmidi::Msg<3>>{2.02,
        bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                    bmmidi::Channel::index(7)),
        bmmidi::DataValue{1},
        bmmidi::DataValue{88}};

    bmmidi::TimedControlChangeMsg msg = genericMsg.to<bmmidi::ControlChangeMsg>();
    EXPECT_THAT(msg.timestamp(), Eq(2.02));
    EXPECT_THAT(msg.value().channel().index(), Eq(7));
    EXPECT_THAT(msg.value().control(), Eq(bmmidi::Control::kModWheel));
    EXPECT_THAT(msg.value().value(), Eq(bmmidi::DataValue{88}));
  }
}

TEST(ProgramChangeMsg, ShouldCreate) {
  // (Program Change, channel 3, program 57).
  auto pcMsg = bmmidi::ProgramChangeMsg{
     bmmidi::Channel::index(2), bmmidi::PresetNumber::index(56)};

  EXPECT_THAT(pcMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(pcMsg.program().displayNumber(), Eq(57));

  EXPECT_THAT(pcMsg.rawBytes()[0], Eq(0xC2));
  EXPECT_THAT(pcMsg.rawBytes()[1], Eq(0x38));

  // Can mutate:
  pcMsg.setChannel(bmmidi::Channel::index(13));
  pcMsg.setProgram(bmmidi::PresetNumber::index(22));

  EXPECT_THAT(pcMsg.channel().displayNumber(), Eq(14));
  EXPECT_THAT(pcMsg.program().displayNumber(), Eq(23));

  EXPECT_THAT(pcMsg.rawBytes()[0], Eq(0xCD));
  EXPECT_THAT(pcMsg.rawBytes()[1], Eq(0x16));
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

TEST(TimedProgramChangeMsg, SupportsFactoriesAndConversion) {
  {
    // Time 1.01, Program Change, Channel 3, Program 57:
    auto msg = bmmidi::TimedProgramChangeMsg{1.01,
        bmmidi::Channel::index(2), bmmidi::PresetNumber::index(56)};
    EXPECT_THAT(msg.timestamp(), Eq(1.01));
    EXPECT_THAT(msg.value().channel().index(), Eq(2));
    EXPECT_THAT(msg.value().program().index(), Eq(56));
  }

  {
    // Should convert from TimedMsg<Msg<N>>:
    auto genericMsg = bmmidi::TimedMsg<bmmidi::Msg<2>>{2.02,
        bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                     bmmidi::Channel::index(7)),
        bmmidi::DataValue{88}};

    bmmidi::TimedProgramChangeMsg msg = genericMsg.to<bmmidi::ProgramChangeMsg>();
    EXPECT_THAT(msg.timestamp(), Eq(2.02));
    EXPECT_THAT(msg.value().channel().index(), Eq(7));
    EXPECT_THAT(msg.value().program().index(), Eq(88));
  }
}

TEST(ChanPressureMsg, ShouldCreate) {
  // (Channel Pressure, channel 3, pressure 56).
  auto cpMsg = bmmidi::ChanPressureMsg{bmmidi::Channel::index(2), bmmidi::DataValue{56}};

  EXPECT_THAT(cpMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(cpMsg.pressure().value(), Eq(56));

  EXPECT_THAT(cpMsg.rawBytes()[0], Eq(0xD2));
  EXPECT_THAT(cpMsg.rawBytes()[1], Eq(0x38));

  // Can mutate:
  cpMsg.setChannel(bmmidi::Channel::index(13));
  cpMsg.setPressure(bmmidi::DataValue{22});

  EXPECT_THAT(cpMsg.channel().displayNumber(), Eq(14));
  EXPECT_THAT(cpMsg.pressure().value(), Eq(22));

  EXPECT_THAT(cpMsg.rawBytes()[0], Eq(0xDD));
  EXPECT_THAT(cpMsg.rawBytes()[1], Eq(0x16));
}

TEST(ChanPressureMsg, ShouldConvertFromMsg) {
  // A more generic Msg...
  // (Channel Pressure, channel 3, pressure 56).
  bmmidi::Msg<2> msg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                   bmmidi::Channel::index(2)),
      bmmidi::DataValue{56}};
  
  // ...should convert to the more specific ChanPressureMsg...
  auto cpMsg = msg.to<bmmidi::ChanPressureMsg>();

  EXPECT_THAT(cpMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(cpMsg.pressure().value(), Eq(56));

  // ...and this converted copy should be mutable...
  cpMsg.setPressure(bmmidi::DataValue{22});
  EXPECT_THAT(cpMsg.pressure().value(), Eq(22));

  // ...and changes to it should NOT affect original Msg.
  EXPECT_THAT(msg.data1().value(), Eq(56));
}

TEST(TimedChanPressureMsg, SupportsFactoriesAndConversion) {
  {
    // Time 1.01, Channel Pressure, Channel 3, Pressure 56.
    auto msg = bmmidi::TimedChanPressureMsg{1.01,
        bmmidi::Channel::index(2), bmmidi::DataValue{56}};
    EXPECT_THAT(msg.timestamp(), Eq(1.01));
    EXPECT_THAT(msg.value().channel().index(), Eq(2));
    EXPECT_THAT(msg.value().pressure().value(), Eq(56));
  }

  {
    // Should convert from TimedMsg<Msg<N>>:
    auto genericMsg = bmmidi::TimedMsg<bmmidi::Msg<2>>{2.02,
        bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                     bmmidi::Channel::index(7)),
        bmmidi::DataValue{88}};

    bmmidi::TimedChanPressureMsg msg = genericMsg.to<bmmidi::ChanPressureMsg>();
    EXPECT_THAT(msg.timestamp(), Eq(2.02));
    EXPECT_THAT(msg.value().channel().index(), Eq(7));
    EXPECT_THAT(msg.value().pressure().value(), Eq(88));
  }
}

TEST(PitchBendMsg, ShouldCreate) {
  // (Pitch Bend, channel 3, bend 3427).
  // LSB:          0b110 0011 (  0x63 = 99)
  // MSB: 0b00 1101 0         (  0x1A = 26)
  // Val: 0b00 1101 0110 0011 (0x0D63 = 3427)
  auto pbMsg = bmmidi::PitchBendMsg{bmmidi::Channel::index(2), bmmidi::PitchBend{3427}};

  EXPECT_THAT(pbMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(pbMsg.bend().value(), Eq(3427));

  EXPECT_THAT(pbMsg.rawBytes()[0], Eq(0xE2));
  EXPECT_THAT(pbMsg.rawBytes()[1], Eq(0x63));
  EXPECT_THAT(pbMsg.rawBytes()[2], Eq(0x1A));

  // Can mutate:
  pbMsg.setChannel(bmmidi::Channel::index(13));
  pbMsg.setBend(bmmidi::PitchBend::max());

  EXPECT_THAT(pbMsg.channel().displayNumber(), Eq(14));
  EXPECT_THAT(pbMsg.bend().value(), Eq(16383));

  EXPECT_THAT(pbMsg.rawBytes()[0], Eq(0xED));
  EXPECT_THAT(pbMsg.rawBytes()[1], Eq(0x7F));
  EXPECT_THAT(pbMsg.rawBytes()[2], Eq(0x7F));
}

TEST(PitchBendMsg, ShouldConvertFromMsg) {
  // A more generic Msg...
  // (Pitch Bend, channel 3, bend 8192).
  // LSB:          0b000 0000 (  0x00 = 0)
  // MSB: 0b10 0000 0         (  0x40 = 64)
  // Val: 0b10 0000 0000 0000 (0x2000 = 8192)
  bmmidi::Msg<3> msg{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kPitchBend,
                                   bmmidi::Channel::index(2)),
      bmmidi::DataValue{0x00},
      bmmidi::DataValue{0x40}};
  
  // ...should convert to the more specific PitchBendMsg...
  auto pbMsg = msg.to<bmmidi::PitchBendMsg>();

  EXPECT_THAT(pbMsg.channel().displayNumber(), Eq(3));
  EXPECT_THAT(pbMsg.bend().value(), Eq(8192));

  // ...and this converted copy should be mutable...
  pbMsg.setBend(bmmidi::PitchBend{22});
  EXPECT_THAT(pbMsg.bend().value(), Eq(22));

  // ...and changes to it should NOT affect original Msg.
  EXPECT_THAT(msg.data1().value(), Eq(0x00));
  EXPECT_THAT(msg.data2().value(), Eq(0x40));
}

TEST(TimedPitchBendMsg, SupportsFactoriesAndConversion) {
  {
    // Time 1.01, Pitch Bend, Channel 3, Bend 377.
    auto msg = bmmidi::TimedPitchBendMsg{1.01,
        bmmidi::Channel::index(2), bmmidi::PitchBend{377}};
    EXPECT_THAT(msg.timestamp(), Eq(1.01));
    EXPECT_THAT(msg.value().channel().index(), Eq(2));
    EXPECT_THAT(msg.value().bend().value(), Eq(377));
  }

  {
    // Should convert from TimedMsg<Msg<N>>:
    // LSB:          0b000 0000 (  0x00 = 0)
    // MSB: 0b10 0000 0         (  0x40 = 64)
    // Val: 0b10 0000 0000 0000 (0x2000 = 8192)
    auto genericMsg = bmmidi::TimedMsg<bmmidi::Msg<3>>{2.02,
        bmmidi::Status::channelVoice(bmmidi::MsgType::kPitchBend,
                                     bmmidi::Channel::index(7)),
        bmmidi::DataValue{0x00},
        bmmidi::DataValue{0x40}};

    bmmidi::TimedPitchBendMsg msg = genericMsg.to<bmmidi::PitchBendMsg>();
    EXPECT_THAT(msg.timestamp(), Eq(2.02));
    EXPECT_THAT(msg.value().channel().index(), Eq(7));
    EXPECT_THAT(msg.value().bend().value(), Eq(8192));
  }
}

TEST(MtcQuarterFrameMsg, ShouldCreateAsPieceOfFullTC) {
  // Set timecode of:
  //   - Rate: 25.000 fps (rr       = 01      )
  //   - HH: 23           (  h'hhhh =   1'0111)
  //   - MM: 59           ( mm'mmmm =  11'1011)
  //   - SS: 07           ( ss'ssss =  00'0111)
  //   - FF: 24           (  f'ffff =   1'1000)
  auto tc = bmmidi::MtcFullFrame::zero(bmmidi::MtcFrameRateStandard::k25NonDrop);
  tc.setHH(23);
  tc.setMM(59);
  tc.setSS(7);
  tc.setFF(24);

  auto qfMsg =
      bmmidi::MtcQuarterFrameMsg::pieceOf(tc, bmmidi::MtcQuarterFramePiece::k4MinLowerBits);

  EXPECT_THAT(qfMsg.dataByte(), Eq(0x4B));
  EXPECT_THAT(qfMsg.piece(), Eq(bmmidi::MtcQuarterFramePiece::k4MinLowerBits));
  EXPECT_THAT(qfMsg.valueInLower4Bits(), Eq(0x0B));

  EXPECT_THAT(qfMsg.rawBytes()[0], Eq(0xF1));
  EXPECT_THAT(qfMsg.rawBytes()[1], Eq(0x4B));
}

TEST(MtcQuarterFrameMsg, ShouldCreateFromDataByte) {
  // (MTC Quarter Frame, MM lower 4 bits of 0b1011).
  // Upper bits identifying piece are 100. Lower bits encode value (for MM lower 4 bits).
  auto qfMsg = bmmidi::MtcQuarterFrameMsg::withDataByte(0b0100'1011);  // 0x4B.

  EXPECT_THAT(qfMsg.dataByte(), Eq(0x4B));
  EXPECT_THAT(qfMsg.piece(), Eq(bmmidi::MtcQuarterFramePiece::k4MinLowerBits));
  EXPECT_THAT(qfMsg.valueInLower4Bits(), Eq(0x0B));

  EXPECT_THAT(qfMsg.rawBytes()[0], Eq(0xF1));
  EXPECT_THAT(qfMsg.rawBytes()[1], Eq(0x4B));

  // Can mutate whole data byte:
  // (Rate + HH 0rrh upper 4 bits of 0b0011).
  qfMsg.setDataByte(0b0111'0011);  // 0x73.

  EXPECT_THAT(qfMsg.dataByte(), Eq(0x73));
  EXPECT_THAT(qfMsg.piece(), Eq(bmmidi::MtcQuarterFramePiece::k7RateHourUpperBits));
  EXPECT_THAT(qfMsg.valueInLower4Bits(), Eq(0x03));

  EXPECT_THAT(qfMsg.rawBytes()[0], Eq(0xF1));
  EXPECT_THAT(qfMsg.rawBytes()[1], Eq(0x73));

  // Or can set piece and value individually:
  // (FF upper 1 bit of 0b0001).
  qfMsg.setPiece(bmmidi::MtcQuarterFramePiece::k1FrameUpperBits);  // 0x10.
  qfMsg.setValueFromLower4Bits(0b0000'0001);  // 0x01.

  EXPECT_THAT(qfMsg.dataByte(), Eq(0x11));
  EXPECT_THAT(qfMsg.piece(), Eq(bmmidi::MtcQuarterFramePiece::k1FrameUpperBits));
  EXPECT_THAT(qfMsg.valueInLower4Bits(), Eq(0x01));

  EXPECT_THAT(qfMsg.rawBytes()[0], Eq(0xF1));
  EXPECT_THAT(qfMsg.rawBytes()[1], Eq(0x11));
}

TEST(MtcQuarterFrameMsg, ShouldConvertFromMsg) {
  // A more generic Msg...
  // (MTC Quarter Frame, MM lower 4 bits of 0b1011).
  // Upper bits identifying piece are 100. Lower bits encode value (for MM lower 4 bits).
  bmmidi::Msg<2> msg{
      bmmidi::Status::system(bmmidi::MsgType::kMtcQuarterFrame),
      bmmidi::DataValue{0b0100'1011}};  // 0x4B.
  
  // ...should convert to the more specific MtcQuarterFrameMsg...
  auto qfMsg = msg.to<bmmidi::MtcQuarterFrameMsg>();

  EXPECT_THAT(qfMsg.dataByte(), Eq(0x4B));
  EXPECT_THAT(qfMsg.piece(), Eq(bmmidi::MtcQuarterFramePiece::k4MinLowerBits));
  EXPECT_THAT(qfMsg.valueInLower4Bits(), Eq(0x0B));

  EXPECT_THAT(qfMsg.rawBytes()[0], Eq(0xF1));
  EXPECT_THAT(qfMsg.rawBytes()[1], Eq(0x4B));

  // ...and this converted copy should be mutable...
  // (Rate + HH 0rrh upper 4 bits of 0b0011).
  qfMsg.setDataByte(0x73);
  EXPECT_THAT(qfMsg.dataByte(), Eq(0x73));

  // ...and changes to it should NOT affect original Msg.
  EXPECT_THAT(msg.data1().value(), Eq(0x4B));
}

TEST(TimedMtcQuarterFrameMsg, SupportsFactoriesAndConversion) {
  {
    // Set timecode of:
    //   - Rate: 25.000 fps (rr       = 01      )
    //   - HH: 23           (  h'hhhh =   1'0111)
    //   - MM: 59           ( mm'mmmm =  11'1011)
    //   - SS: 07           ( ss'ssss =  00'0111)
    //   - FF: 24           (  f'ffff =   1'1000)
    auto tc = bmmidi::MtcFullFrame::zero(bmmidi::MtcFrameRateStandard::k25NonDrop);
    tc.setHH(23);
    tc.setMM(59);
    tc.setSS(7);
    tc.setFF(24);

    // Time 1.01, MTC Quarter Frame, Lower 4 bits of MM:
    auto msg = bmmidi::TimedMtcQuarterFrameMsg::pieceOf(1.01,
        tc, bmmidi::MtcQuarterFramePiece::k4MinLowerBits);
    EXPECT_THAT(msg.timestamp(), Eq(1.01));
    EXPECT_THAT(msg.value().dataByte(), Eq(0x4B));
    EXPECT_THAT(msg.value().piece(), Eq(bmmidi::MtcQuarterFramePiece::k4MinLowerBits));
    EXPECT_THAT(msg.value().valueInLower4Bits(), Eq(0x0B));
  }

  {
    // Time 2.02, MTC Quarter Frame, MM lower 4 bits of 0b1011.
    // Upper bits identifying piece are 100. Lower bits encode value (for MM lower 4 bits).
    auto msg = bmmidi::TimedMtcQuarterFrameMsg::withDataByte(2.02, 0b0100'1011);  // 0x4B.
    EXPECT_THAT(msg.timestamp(), Eq(2.02));
    EXPECT_THAT(msg.value().dataByte(), Eq(0x4B));
    EXPECT_THAT(msg.value().piece(), Eq(bmmidi::MtcQuarterFramePiece::k4MinLowerBits));
    EXPECT_THAT(msg.value().valueInLower4Bits(), Eq(0x0B));
  }

  {
    // Should convert from TimedMsg<Msg<N>>:
    auto genericMsg = bmmidi::TimedMsg<bmmidi::Msg<2>>{3.03,
        bmmidi::Status::system(bmmidi::MsgType::kMtcQuarterFrame),
        bmmidi::DataValue{0b0100'1011}};  // 0x4B.

    bmmidi::TimedMtcQuarterFrameMsg msg = genericMsg.to<bmmidi::MtcQuarterFrameMsg>();
    EXPECT_THAT(msg.timestamp(), Eq(3.03));
    EXPECT_THAT(msg.value().dataByte(), Eq(0x4B));
    EXPECT_THAT(msg.value().piece(), Eq(bmmidi::MtcQuarterFramePiece::k4MinLowerBits));
    EXPECT_THAT(msg.value().valueInLower4Bits(), Eq(0x0B));
  }
}

TEST(SongPosMsg, ShouldCreate) {
  // (Song Position Pointer, 3427 sixteenths after start).
  // LSB:          0b110 0011 (  0x63 = 99)
  // MSB: 0b00 1101 0         (  0x1A = 26)
  // Val: 0b00 1101 0110 0011 (0x0D63 = 3427)
  auto posMsg = bmmidi::SongPosMsg::atSixteenthsAfterStart(bmmidi::DoubleDataValue{3427});

  EXPECT_THAT(posMsg.sixteenthsAfterStart().value(), Eq(3427));

  EXPECT_THAT(posMsg.rawBytes()[0], Eq(0xF2));
  EXPECT_THAT(posMsg.rawBytes()[1], Eq(0x63));
  EXPECT_THAT(posMsg.rawBytes()[2], Eq(0x1A));

  // Can mutate:
  posMsg.setSixteenthsAfterStart(bmmidi::DoubleDataValue::max());

  EXPECT_THAT(posMsg.sixteenthsAfterStart().value(), Eq(16383));

  EXPECT_THAT(posMsg.rawBytes()[0], Eq(0xF2));
  EXPECT_THAT(posMsg.rawBytes()[1], Eq(0x7F));
  EXPECT_THAT(posMsg.rawBytes()[2], Eq(0x7F));
}

TEST(SongPosMsg, ShouldConvertFromMsg) {
  // A more generic Msg...
  // (Song Position Pointer, 8192 sixteenths after start).
  // LSB:          0b000 0000 (  0x00 = 0)
  // MSB: 0b10 0000 0         (  0x40 = 64)
  // Val: 0b10 0000 0000 0000 (0x2000 = 8192)
  bmmidi::Msg<3> msg{
      bmmidi::Status::system(bmmidi::MsgType::kSongPositionPointer),
      bmmidi::DataValue{0x00},
      bmmidi::DataValue{0x40}};

  // ...should convert to the more specific SongPosMsg...
  auto posMsg = msg.to<bmmidi::SongPosMsg>();

  EXPECT_THAT(posMsg.sixteenthsAfterStart().value(), Eq(8192));

  // ...and this converted copy should be mutable...
  posMsg.setSixteenthsAfterStart(bmmidi::DoubleDataValue{22});
  EXPECT_THAT(posMsg.sixteenthsAfterStart().value(), Eq(22));

  // ...and changes to it should NOT affect original Msg.
  EXPECT_THAT(msg.data1().value(), Eq(0x00));
  EXPECT_THAT(msg.data2().value(), Eq(0x40));
}

TEST(TimedSongPosMsg, SupportsFactoriesAndConversion) {
  {
    // Time 1.01, Song Position Pointer 3427:
    auto msg = bmmidi::TimedSongPosMsg::atSixteenthsAfterStart(1.01, bmmidi::DoubleDataValue{3427});
    EXPECT_THAT(msg.timestamp(), Eq(1.01));
    EXPECT_THAT(msg.value().sixteenthsAfterStart().value(), Eq(3427));
  }

  {
    // Should convert from TimedMsg<Msg<N>>:
    // (Song Position Pointer, 8192 sixteenths after start).
    // LSB:          0b000 0000 (  0x00 = 0)
    // MSB: 0b10 0000 0         (  0x40 = 64)
    // Val: 0b10 0000 0000 0000 (0x2000 = 8192)
    auto genericMsg = bmmidi::TimedMsg<bmmidi::Msg<3>>{2.02,
        bmmidi::Status::system(bmmidi::MsgType::kSongPositionPointer),
        bmmidi::DataValue{0x00},
        bmmidi::DataValue{0x40}};

    bmmidi::TimedSongPosMsg msg = genericMsg.to<bmmidi::SongPosMsg>();
    EXPECT_THAT(msg.timestamp(), Eq(2.02));
    EXPECT_THAT(msg.value().sixteenthsAfterStart().value(), Eq(8192));
  }
}

TEST(SongSelectMsg, ShouldCreate) {
  // (Song Select, song 57).
  auto songMsg = bmmidi::SongSelectMsg{bmmidi::PresetNumber::index(56)};
  EXPECT_THAT(songMsg.song().displayNumber(), Eq(57));

  EXPECT_THAT(songMsg.rawBytes()[0], Eq(0xF3));
  EXPECT_THAT(songMsg.rawBytes()[1], Eq(0x38));

  // Can mutate:
  songMsg.setSong(bmmidi::PresetNumber::index(22));

  EXPECT_THAT(songMsg.song().displayNumber(), Eq(23));
  EXPECT_THAT(songMsg.rawBytes()[0], Eq(0xF3));
  EXPECT_THAT(songMsg.rawBytes()[1], Eq(0x16));
}

TEST(SongSelectMsg, ShouldConvertFromMsg) {
  // A more generic Msg...
  // (Song Select, song 57).
  bmmidi::Msg<2> msg{
      bmmidi::Status::system(bmmidi::MsgType::kSongSelect),
      bmmidi::DataValue{56}};
  
  // ...should convert to the more specific SongSelectMsg...
  auto songMsg = msg.to<bmmidi::SongSelectMsg>();
  EXPECT_THAT(songMsg.song().displayNumber(), Eq(57));

  // ...and this converted copy should be mutable...
  songMsg.setSong(bmmidi::PresetNumber::index(22));
  EXPECT_THAT(songMsg.song().displayNumber(), Eq(23));

  // ...and changes to it should NOT affect original Msg.
  EXPECT_THAT(msg.data1().value(), Eq(56));
}

TEST(TimedSongSelectMsg, SupportsFactoriesAndConversion) {
  {
    // Time 1.01, Song Select, Song 57.
    auto msg = bmmidi::TimedSongSelectMsg{1.01, bmmidi::PresetNumber::index(56)};
    EXPECT_THAT(msg.timestamp(), Eq(1.01));
    EXPECT_THAT(msg.value().song().index(), Eq(56));
  }

  {
    // Should convert from TimedMsg<Msg<N>>:
    auto genericMsg = bmmidi::TimedMsg<bmmidi::Msg<2>>{2.02,
        bmmidi::Status::system(bmmidi::MsgType::kSongSelect),
        bmmidi::DataValue{88}};

    bmmidi::TimedSongSelectMsg msg = genericMsg.to<bmmidi::SongSelectMsg>();
    EXPECT_THAT(msg.timestamp(), Eq(2.02));
    EXPECT_THAT(msg.value().song().index(), Eq(88));
  }
}

TEST(StatusOnlyMsgs, ShouldCreate) {
  // (Oscillator Tune Request).
  const auto tuneMsg = bmmidi::oscTuneMsg();
  EXPECT_THAT(tuneMsg.status(),
      Eq(bmmidi::Status::system(bmmidi::MsgType::kOscillatorTuneRequest)));
  EXPECT_THAT(tuneMsg.rawBytes()[0], Eq(0xF6));

  // (Timing Clock).
  const auto clockMsg = bmmidi::timingClockMsg();
  EXPECT_THAT(clockMsg.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kTimingClock)));
  EXPECT_THAT(clockMsg.rawBytes()[0], Eq(0xF8));

  // (Start Playback).
  const auto playMsg = bmmidi::startPlaybackMsg();
  EXPECT_THAT(playMsg.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kStart)));
  EXPECT_THAT(playMsg.rawBytes()[0], Eq(0xFA));

  // (Continue Playback).
  const auto contMsg = bmmidi::continuePlaybackMsg();
  EXPECT_THAT(contMsg.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kContinue)));
  EXPECT_THAT(contMsg.rawBytes()[0], Eq(0xFB));

  // (Stop Playback).
  const auto stopMsg = bmmidi::stopPlaybackMsg();
  EXPECT_THAT(stopMsg.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kStop)));
  EXPECT_THAT(stopMsg.rawBytes()[0], Eq(0xFC));

  // (Active Sensing).
  const auto senseMsg = bmmidi::activeSensingMsg();
  EXPECT_THAT(senseMsg.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kActiveSensing)));
  EXPECT_THAT(senseMsg.rawBytes()[0], Eq(0xFE));

  // (System Reset).
  const auto resetMsg = bmmidi::systemResetMsg();
  EXPECT_THAT(resetMsg.status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kSystemReset)));
  EXPECT_THAT(resetMsg.rawBytes()[0], Eq(0xFF));
}

TEST(StatusOnlyMsgs, ShouldCreateWithTimestamps) {
  // (Oscillator Tune Request).
  const auto tuneMsg = bmmidi::timedOscTuneMsg(1.01);
  EXPECT_THAT(tuneMsg.timestamp(), Eq(1.01));
  EXPECT_THAT(tuneMsg.value().status(),
      Eq(bmmidi::Status::system(bmmidi::MsgType::kOscillatorTuneRequest)));
  EXPECT_THAT(tuneMsg.value().rawBytes()[0], Eq(0xF6));

  // (Timing Clock).
  const auto clockMsg = bmmidi::timedTimingClockMsg(2.02);
  EXPECT_THAT(clockMsg.timestamp(), Eq(2.02));
  EXPECT_THAT(clockMsg.value().status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kTimingClock)));
  EXPECT_THAT(clockMsg.value().rawBytes()[0], Eq(0xF8));

  // (Start Playback).
  const auto playMsg = bmmidi::timedStartPlaybackMsg(3.03);
  EXPECT_THAT(playMsg.timestamp(), Eq(3.03));
  EXPECT_THAT(playMsg.value().status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kStart)));
  EXPECT_THAT(playMsg.value().rawBytes()[0], Eq(0xFA));

  // (Continue Playback).
  const auto contMsg = bmmidi::timedContinuePlaybackMsg(4.04);
  EXPECT_THAT(contMsg.timestamp(), Eq(4.04));
  EXPECT_THAT(contMsg.value().status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kContinue)));
  EXPECT_THAT(contMsg.value().rawBytes()[0], Eq(0xFB));

  // (Stop Playback).
  const auto stopMsg = bmmidi::timedStopPlaybackMsg(5.05);
  EXPECT_THAT(stopMsg.timestamp(), Eq(5.05));
  EXPECT_THAT(stopMsg.value().status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kStop)));
  EXPECT_THAT(stopMsg.value().rawBytes()[0], Eq(0xFC));

  // (Active Sensing).
  const auto senseMsg = bmmidi::timedActiveSensingMsg(6.06);
  EXPECT_THAT(senseMsg.timestamp(), Eq(6.06));
  EXPECT_THAT(senseMsg.value().status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kActiveSensing)));
  EXPECT_THAT(senseMsg.value().rawBytes()[0], Eq(0xFE));

  // (System Reset).
  const auto resetMsg = bmmidi::timedSystemResetMsg(7.07);
  EXPECT_THAT(resetMsg.timestamp(), Eq(7.07));
  EXPECT_THAT(resetMsg.value().status(), Eq(bmmidi::Status::system(bmmidi::MsgType::kSystemReset)));
  EXPECT_THAT(resetMsg.value().rawBytes()[0], Eq(0xFF));
}

}  // namespace
