#include "bmmidi/timed.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bmmidi/control.hpp"
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
  // (Note: TimedMsg alias declared in msg.hpp).
  auto timedMsg = bmmidi::TimedMsg<2>{
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
  bmmidi::TimedMsg<2> copy{timedMsg};
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
  auto timedCcCh10Expr76 = bmmidi::TimedMsg<3>{
      100.0,
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(9)),
      bmmidi::controlToDataValue(bmmidi::Control::kExpression),
      bmmidi::DataValue{76}};
  auto sameValue = bmmidi::TimedMsg<3>{
      100.0,
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(9)),
      bmmidi::controlToDataValue(bmmidi::Control::kExpression),
      bmmidi::DataValue{76}};
  auto withDifferentTimestamp = bmmidi::TimedMsg<3>{
      100.1,
      bmmidi::Status::channelVoice(bmmidi::MsgType::kControlChange,
                                   bmmidi::Channel::index(9)),
      bmmidi::controlToDataValue(bmmidi::Control::kExpression),
      bmmidi::DataValue{76}};
  auto withDifferentCcValue = bmmidi::TimedMsg<3>{
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

}  // namespace
