#include "bmmidi/timed.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bmmidi/control.hpp"
#include "bmmidi/msg_reference.hpp"
#include "bmmidi/msg.hpp"

namespace {

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;

TEST(Timed, WrapsAnInt) {
  bmmidi::Timed<int> timedInt{3.0, 42};
  EXPECT_THAT(timedInt.timestamp(), Eq(3.0));
  EXPECT_THAT(timedInt.value(), Eq(42));

  timedInt.setTimestamp(5.1);
  EXPECT_THAT(timedInt.timestamp(), Eq(5.1));
  EXPECT_THAT(timedInt.value(), Eq(42));

  timedInt.value() = 3000;
  EXPECT_THAT(timedInt.timestamp(), Eq(5.1));
  EXPECT_THAT(timedInt.value(), Eq(3000));

  // Accessors should be const:
  const auto& constRef = timedInt;
  EXPECT_THAT(constRef.timestamp(), Eq(5.1));
  EXPECT_THAT(constRef.value(), Eq(3000));

  // The following shouldn't compile if uncommented:
  //     constRef.setTimestamp(0.1234);
  //     constRef.value() = 0xBAD;

  // Should support copy construction and assignment.
  bmmidi::Timed<int> copy{timedInt};
  EXPECT_THAT(copy.timestamp(), Eq(5.1));
  EXPECT_THAT(copy.value(), Eq(3000));

  // Change the copy.
  copy.setTimestamp(8675);
  copy.value() = 309;
  EXPECT_THAT(copy.timestamp(), Eq(8675));
  EXPECT_THAT(copy.value(), Eq(309));

  // Original should be unaffected.
  EXPECT_THAT(timedInt.timestamp(), Eq(5.1));
  EXPECT_THAT(timedInt.value(), Eq(3000));

  // Can also re-assign value.
  timedInt = copy;
  EXPECT_THAT(timedInt.timestamp(), Eq(8675));
  EXPECT_THAT(timedInt.value(), Eq(309));
}

TEST(Timed, WrapsMsg) {
  auto timedMsg = bmmidi::Timed<bmmidi::Msg<2>>{
      3.0,
      bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                   bmmidi::Channel::index(12)),
      bmmidi::DataValue{57}};
  EXPECT_THAT(timedMsg.timestamp(), Eq(3.0));
  EXPECT_THAT(timedMsg.value(),
      Eq(bmmidi::Msg<2>{
             bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                          bmmidi::Channel::index(12)),
             bmmidi::DataValue{57}}));

  timedMsg.setTimestamp(5.1);
  EXPECT_THAT(timedMsg.timestamp(), Eq(5.1));
  EXPECT_THAT(timedMsg.value(),
      Eq(bmmidi::Msg<2>{
             bmmidi::Status::channelVoice(bmmidi::MsgType::kProgramChange,
                                          bmmidi::Channel::index(12)),
             bmmidi::DataValue{57}}));

  timedMsg.value() = bmmidi::Msg<2>{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                   bmmidi::Channel::index(3)),
      bmmidi::DataValue{96}};
  EXPECT_THAT(timedMsg.timestamp(), Eq(5.1));
  EXPECT_THAT(timedMsg.value(),
      Eq(bmmidi::Msg<2>{
             bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                          bmmidi::Channel::index(3)),
             bmmidi::DataValue{96}}));

  // Accessors should be const:
  const auto& constRef = timedMsg;
  EXPECT_THAT(constRef.timestamp(), Eq(5.1));
  EXPECT_THAT(constRef.value(),
      Eq(bmmidi::Msg<2>{
             bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                          bmmidi::Channel::index(3)),
             bmmidi::DataValue{96}}));

  // The following shouldn't compile if uncommented:
  //     constRef.setTimestamp(0.1234);
  //     constRef.value() = bmmidi::Msg<2>{
  //         bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
  //                                      bmmidi::Channel::index(3)),
  //         bmmidi::DataValue{96}};

  // Should support copy construction and assignment.
  bmmidi::Timed<bmmidi::Msg<2>> copy{timedMsg};
  EXPECT_THAT(copy.timestamp(), Eq(5.1));
  EXPECT_THAT(copy.value(),
      Eq(bmmidi::Msg<2>{
             bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                          bmmidi::Channel::index(3)),
             bmmidi::DataValue{96}}));

  // Change the copy.
  copy.setTimestamp(8675);
  copy.value() = bmmidi::Msg<2>{
      bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                   bmmidi::Channel::index(1)),
      bmmidi::DataValue{23}};
  EXPECT_THAT(copy.timestamp(), Eq(8675));
  EXPECT_THAT(copy.value(),
      Eq(bmmidi::Msg<2>{
             bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                          bmmidi::Channel::index(1)),
             bmmidi::DataValue{23}}));

  // Original should be unaffected.
  EXPECT_THAT(timedMsg.timestamp(), Eq(5.1));
  EXPECT_THAT(timedMsg.value(),
      Eq(bmmidi::Msg<2>{
             bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                          bmmidi::Channel::index(3)),
             bmmidi::DataValue{96}}));

  // Can also re-assign value.
  timedMsg = copy;
  EXPECT_THAT(timedMsg.timestamp(), Eq(8675));
  EXPECT_THAT(timedMsg.value(),
      Eq(bmmidi::Msg<2>{
             bmmidi::Status::channelVoice(bmmidi::MsgType::kChannelPressure,
                                          bmmidi::Channel::index(1)),
             bmmidi::DataValue{23}}));
}

TEST(Timed, SupportsEqualityOperations) {
  auto timedCcCh10Expr76 = bmmidi::Timed<bmmidi::Msg<3>>{
      100.0,
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(9)),
      bmmidi::controlToDataValue(bmmidi::Control::kExpression),
      bmmidi::DataValue{76}};
  auto sameValue = bmmidi::Timed<bmmidi::Msg<3>>{
      100.0,
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(9)),
      bmmidi::controlToDataValue(bmmidi::Control::kExpression),
      bmmidi::DataValue{76}};
  auto withDifferentTimestamp = bmmidi::Timed<bmmidi::Msg<3>>{
      100.1,
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(9)),
      bmmidi::controlToDataValue(bmmidi::Control::kExpression),
      bmmidi::DataValue{76}};
  auto withDifferentCcValue = bmmidi::Timed<bmmidi::Msg<3>>{
      100.0,
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(9)),
      bmmidi::controlToDataValue(bmmidi::Control::kExpression),
      bmmidi::DataValue{88}};

  EXPECT_THAT(timedCcCh10Expr76 == timedCcCh10Expr76, IsTrue());
  EXPECT_THAT(timedCcCh10Expr76 != timedCcCh10Expr76, IsFalse());

  EXPECT_THAT(timedCcCh10Expr76 == sameValue, IsTrue());
  EXPECT_THAT(sameValue == timedCcCh10Expr76, IsTrue());
  EXPECT_THAT(timedCcCh10Expr76 != sameValue, IsFalse());
  EXPECT_THAT(sameValue != timedCcCh10Expr76, IsFalse());

  EXPECT_THAT(timedCcCh10Expr76 == withDifferentTimestamp, IsFalse());
  EXPECT_THAT(withDifferentTimestamp == timedCcCh10Expr76, IsFalse());
  EXPECT_THAT(timedCcCh10Expr76 != withDifferentTimestamp, IsTrue());
  EXPECT_THAT(withDifferentTimestamp != timedCcCh10Expr76, IsTrue());

  EXPECT_THAT(timedCcCh10Expr76 == withDifferentCcValue, IsFalse());
  EXPECT_THAT(withDifferentCcValue == timedCcCh10Expr76, IsFalse());
  EXPECT_THAT(timedCcCh10Expr76 != withDifferentCcValue, IsTrue());
  EXPECT_THAT(withDifferentCcValue != timedCcCh10Expr76, IsTrue());
}

TEST(TimedMsg, SupportsMsgToMsgConversions) {
  // A more generic TimedMsg...
  // (Note On, channel 3, key 69, velocity 90, at time 654.321).
  bmmidi::TimedMsg<bmmidi::Msg<3>> timedMsg{
      654.321,
      bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                   bmmidi::Channel::index(2)),
      bmmidi::DataValue{69},
      bmmidi::DataValue{90}};

  // ...should convert to the more specific TimedNoteMsg.
  bmmidi::TimedNoteMsg timedNoteMsg = timedMsg.to<bmmidi::NoteMsg>();
  EXPECT_THAT(timedNoteMsg.timestamp(), Eq(654.321));

  EXPECT_THAT(timedNoteMsg.value().channel().displayNumber(), Eq(3));
  EXPECT_THAT(timedNoteMsg.value().isNoteOff(), IsFalse());
  EXPECT_THAT(timedNoteMsg.value().isNoteOn(), IsTrue());
  EXPECT_THAT(timedNoteMsg.value().key().value(), Eq(69));
  EXPECT_THAT(timedNoteMsg.value().velocity().value(), Eq(90));

  // ...and this converted copy should be mutable...
  timedNoteMsg.setTimestamp(987.654);
  EXPECT_THAT(timedNoteMsg.timestamp(), Eq(987.654));
  timedNoteMsg.value().setVelocity(bmmidi::DataValue{23});
  EXPECT_THAT(timedNoteMsg.value().velocity().value(), Eq(23));

  // ...and changes to it should NOT affect original Msg.
  EXPECT_THAT(timedMsg.timestamp(), Eq(654.321));
  EXPECT_THAT(timedMsg.value().data2().value(), Eq(90));
}

TEST(TimedMsg, SupportsMsgToReferenceConversions) {
  // A more generic TimedMsg...
  // (Note On, channel 3, key 69, velocity 90, at time 654.321).
  bmmidi::TimedMsg<bmmidi::Msg<3>> timedMsg{
      654.321,
      bmmidi::Status::channelVoice(bmmidi::MsgType::kNoteOn,
                                   bmmidi::Channel::index(2)),
      bmmidi::DataValue{69},
      bmmidi::DataValue{90}};

  // ...should convert to the more specific read-only TimedNoteMsgView...
  bmmidi::TimedNoteMsgView timedNoteMsgView = timedMsg.asView<bmmidi::NoteMsgView>();
  EXPECT_THAT(timedNoteMsgView.timestamp(), Eq(654.321));

  EXPECT_THAT(timedNoteMsgView.value().channel().displayNumber(), Eq(3));
  EXPECT_THAT(timedNoteMsgView.value().isNoteOff(), IsFalse());
  EXPECT_THAT(timedNoteMsgView.value().isNoteOn(), IsTrue());
  EXPECT_THAT(timedNoteMsgView.value().key().value(), Eq(69));
  EXPECT_THAT(timedNoteMsgView.value().velocity().value(), Eq(90));

  // ...or to the more specific read-write TimedNoteMsgRef...
  bmmidi::TimedNoteMsgRef timedNoteMsgRef = timedMsg.asRef<bmmidi::NoteMsgRef>();
  EXPECT_THAT(timedNoteMsgRef.timestamp(), Eq(654.321));

  EXPECT_THAT(timedNoteMsgRef.value().channel().displayNumber(), Eq(3));
  EXPECT_THAT(timedNoteMsgRef.value().isNoteOff(), IsFalse());
  EXPECT_THAT(timedNoteMsgRef.value().isNoteOn(), IsTrue());
  EXPECT_THAT(timedNoteMsgRef.value().key().value(), Eq(69));
  EXPECT_THAT(timedNoteMsgRef.value().velocity().value(), Eq(90));

  // ...which should be support mutations...
  timedNoteMsgRef.value().setVelocity(bmmidi::DataValue{23});
  EXPECT_THAT(timedNoteMsgRef.value().velocity().value(), Eq(23));

  // ...that affect the original TimedMsg...
  EXPECT_THAT(timedMsg.value().data2().value(), Eq(23));

  // ...except for timestamp, which is local to each Timed<> wrapped value.
  timedNoteMsgRef.setTimestamp(987.654);
  EXPECT_THAT(timedNoteMsgRef.timestamp(), Eq(987.654));
  EXPECT_THAT(timedMsg.timestamp(), Eq(654.321));
}

TEST(TimedMsg, SupportsReferenceToReferenceConversions) {
  // A more generic TimedMsg<MsgRef> (which is read-write)...
  // (Note On, channel 3, key 69, velocity 90).
  std::uint8_t srcBytes[] = {0x92, 0x45, 0x5A};
  bmmidi::TimedMsgRef timedRef{654.321, srcBytes, static_cast<int>(sizeof(srcBytes))};

  // ...should convert to the more specific read-only TimedNoteMsgView...
  bmmidi::TimedNoteMsgView timedNoteMsgView = timedRef.asView<bmmidi::NoteMsgView>();
  EXPECT_THAT(timedNoteMsgView.timestamp(), Eq(654.321));

  EXPECT_THAT(timedNoteMsgView.value().channel().displayNumber(), Eq(3));
  EXPECT_THAT(timedNoteMsgView.value().isNoteOff(), IsFalse());
  EXPECT_THAT(timedNoteMsgView.value().isNoteOn(), IsTrue());
  EXPECT_THAT(timedNoteMsgView.value().key().value(), Eq(69));
  EXPECT_THAT(timedNoteMsgView.value().velocity().value(), Eq(90));

  // ...or to the more specific read-write TimedNoteMsgRef...
  bmmidi::TimedNoteMsgRef timedNoteMsgRef = timedRef.asRef<bmmidi::NoteMsgRef>();
  EXPECT_THAT(timedNoteMsgRef.timestamp(), Eq(654.321));

  EXPECT_THAT(timedNoteMsgRef.value().channel().displayNumber(), Eq(3));
  EXPECT_THAT(timedNoteMsgRef.value().isNoteOff(), IsFalse());
  EXPECT_THAT(timedNoteMsgRef.value().isNoteOn(), IsTrue());
  EXPECT_THAT(timedNoteMsgRef.value().key().value(), Eq(69));
  EXPECT_THAT(timedNoteMsgRef.value().velocity().value(), Eq(90));

  // ...which should be support mutations...
  timedNoteMsgRef.value().setVelocity(bmmidi::DataValue{23});
  EXPECT_THAT(timedNoteMsgRef.value().velocity().value(), Eq(23));

  // ...that affect the original TimedMsg...
  EXPECT_THAT(timedRef.value().data2().value(), Eq(23));

  // ...except for timestamp, which is local to each Timed<> wrapped value.
  timedNoteMsgRef.setTimestamp(987.654);
  EXPECT_THAT(timedNoteMsgRef.timestamp(), Eq(987.654));
  EXPECT_THAT(timedRef.timestamp(), Eq(654.321));
}

}  // namespace
