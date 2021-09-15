#include "bmmidi/sysex.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;

// Should support constexpr:
constexpr auto kSeqCircuits = bmmidi::Manufacturer::shortId(0x01);
constexpr auto kSpectrasonics = bmmidi::Manufacturer::extId(0x02, 0x2C);
constexpr auto kNonCommercial = bmmidi::Manufacturer::nonCommercial();

TEST(Manufacturer, SupportsShortId) {
  EXPECT_THAT(kSeqCircuits.isExtended(), IsFalse());
  EXPECT_THAT(kSeqCircuits.sysExId(), Eq(0x01));
}

TEST(Manufacturer, SupportsExtendedId) {
  EXPECT_THAT(kSpectrasonics.isExtended(), IsTrue());
  EXPECT_THAT(kSpectrasonics.sysExId(), Eq(0x00));
  EXPECT_THAT(kSpectrasonics.extByte1(), Eq(0x02));
  EXPECT_THAT(kSpectrasonics.extByte2(), Eq(0x2C));
}

TEST(Manufacturer, SupportsNonCommercial) {
  EXPECT_THAT(kNonCommercial.isExtended(), IsFalse());
  EXPECT_THAT(kNonCommercial.sysExId(), Eq(0x7D));
}

TEST(Manufacturer, SupportsEqualityOperations) {
  EXPECT_THAT(kSeqCircuits == kSeqCircuits, IsTrue());
  EXPECT_THAT(kSeqCircuits != kSeqCircuits, IsFalse());

  EXPECT_THAT(kSpectrasonics == kSpectrasonics, IsTrue());
  EXPECT_THAT(kSpectrasonics != kSpectrasonics, IsFalse());

  EXPECT_THAT(kNonCommercial == kNonCommercial, IsTrue());
  EXPECT_THAT(kNonCommercial != kNonCommercial, IsFalse());

  EXPECT_THAT(kSpectrasonics == kSeqCircuits, IsFalse());
  EXPECT_THAT(kSeqCircuits == kSpectrasonics, IsFalse());
  EXPECT_THAT(kSpectrasonics != kSeqCircuits, IsTrue());
  EXPECT_THAT(kSeqCircuits != kSpectrasonics, IsTrue());

  EXPECT_THAT(kNonCommercial == kSeqCircuits, IsFalse());
  EXPECT_THAT(kSeqCircuits == kNonCommercial, IsFalse());
  EXPECT_THAT(kNonCommercial != kSeqCircuits, IsTrue());
  EXPECT_THAT(kSeqCircuits != kNonCommercial, IsTrue());

  const auto alsoSeqCircuits = bmmidi::Manufacturer::shortId(0x01);
  EXPECT_THAT(kSeqCircuits == alsoSeqCircuits, IsTrue());
  EXPECT_THAT(alsoSeqCircuits == kSeqCircuits, IsTrue());
  EXPECT_THAT(kSeqCircuits != alsoSeqCircuits, IsFalse());
  EXPECT_THAT(alsoSeqCircuits != kSeqCircuits, IsFalse());
}

TEST(UniversalType, ProvidesCategoryAndSubIds) {
  EXPECT_THAT(bmmidi::universal::kSampleDumpHeader.category(),
              Eq(bmmidi::UniversalCategory::kNonRealTime));
  EXPECT_THAT(bmmidi::universal::kSampleDumpHeader.subId1(), Eq(0x01));
  EXPECT_THAT(bmmidi::universal::kSampleDumpHeader.hasSubId2(), IsFalse());

  EXPECT_THAT(bmmidi::universal::kTuningBulkDumpReq.category(),
              Eq(bmmidi::UniversalCategory::kNonRealTime));
  EXPECT_THAT(bmmidi::universal::kTuningBulkDumpReq.subId1(), Eq(0x08));
  EXPECT_THAT(bmmidi::universal::kTuningBulkDumpReq.hasSubId2(), IsTrue());
  EXPECT_THAT(bmmidi::universal::kTuningBulkDumpReq.subId2(), Eq(0x00));

  EXPECT_THAT(bmmidi::universal::kMtcFull.category(),
              Eq(bmmidi::UniversalCategory::kRealTime));
  EXPECT_THAT(bmmidi::universal::kMtcFull.subId1(), Eq(0x01));
  EXPECT_THAT(bmmidi::universal::kMtcFull.hasSubId2(), IsTrue());
  EXPECT_THAT(bmmidi::universal::kMtcFull.subId2(), Eq(0x01));
}

TEST(UniversalType, SupportsEqualityOperations) {
  const auto sampleDumpHeader = bmmidi::UniversalType::nonRealTime(0x01);

  EXPECT_THAT(sampleDumpHeader == sampleDumpHeader, IsTrue());
  EXPECT_THAT(sampleDumpHeader != sampleDumpHeader, IsFalse());

  EXPECT_THAT(sampleDumpHeader == bmmidi::universal::kSampleDumpHeader, IsTrue());
  EXPECT_THAT(bmmidi::universal::kSampleDumpHeader == sampleDumpHeader, IsTrue());
  EXPECT_THAT(sampleDumpHeader != bmmidi::universal::kSampleDumpHeader, IsFalse());
  EXPECT_THAT(bmmidi::universal::kSampleDumpHeader != sampleDumpHeader, IsFalse());

  EXPECT_THAT(bmmidi::universal::kTuningNonRtOct1B == bmmidi::universal::kTuningRtOct1B, IsFalse());
  EXPECT_THAT(bmmidi::universal::kTuningRtOct1B == bmmidi::universal::kTuningNonRtOct1B, IsFalse());
  EXPECT_THAT(bmmidi::universal::kTuningNonRtOct1B != bmmidi::universal::kTuningRtOct1B, IsTrue());
  EXPECT_THAT(bmmidi::universal::kTuningRtOct1B != bmmidi::universal::kTuningNonRtOct1B, IsTrue());
}

TEST(Device, ProvidesAllCallDevice) {
  const auto allDevice = bmmidi::Device::all();

  EXPECT_THAT(allDevice.isAll(), IsTrue());
  EXPECT_THAT(allDevice.value(), Eq(127));
}

TEST(Device, SupportsSpecificDeviceIds) {
  const auto device32 = bmmidi::Device::id(32);

  EXPECT_THAT(device32.isAll(), IsFalse());
  EXPECT_THAT(device32.value(), Eq(32));
}

TEST(Device, SupportsEqualityOperations) {
  const auto allDevice = bmmidi::Device::all();
  const auto alsoAllDevice = bmmidi::Device::all();

  const auto device32 = bmmidi::Device::id(32);
  const auto alsoDevice32 = bmmidi::Device::id(32);

  const auto device33 = bmmidi::Device::id(33);

  EXPECT_THAT(allDevice == allDevice, IsTrue());
  EXPECT_THAT(allDevice != allDevice, IsFalse());

  EXPECT_THAT(device32 == device32, IsTrue());
  EXPECT_THAT(device32 != device32, IsFalse());

  EXPECT_THAT(allDevice == alsoAllDevice, IsTrue());
  EXPECT_THAT(alsoAllDevice == allDevice, IsTrue());
  EXPECT_THAT(allDevice != alsoAllDevice, IsFalse());
  EXPECT_THAT(alsoAllDevice != allDevice, IsFalse());

  EXPECT_THAT(device32 == alsoDevice32, IsTrue());
  EXPECT_THAT(alsoDevice32 == device32, IsTrue());
  EXPECT_THAT(device32 != alsoDevice32, IsFalse());
  EXPECT_THAT(alsoDevice32 != device32, IsFalse());

  EXPECT_THAT(device32 == allDevice, IsFalse());
  EXPECT_THAT(allDevice == device32, IsFalse());
  EXPECT_THAT(device32 != allDevice, IsTrue());
  EXPECT_THAT(allDevice != device32, IsTrue());

  EXPECT_THAT(device32 == device33, IsFalse());
  EXPECT_THAT(device33 == device32, IsFalse());
  EXPECT_THAT(device32 != device33, IsTrue());
  EXPECT_THAT(device33 != device32, IsTrue());
}

TEST(MfrSysEx, WorksForShortIdOnHeap) {
  auto builder = bmmidi::MfrSysExBuilder{kSeqCircuits}.withNumPayloadBytes(4);
  EXPECT_THAT(builder.numMsgBytesIncludingEox(), Eq(7));

  auto msg = builder.buildOnHeap();
  EXPECT_THAT(msg.manufacturer(), Eq(kSeqCircuits));

  // Can fill in 4 payload bytes (with some fake payload data).
  EXPECT_THAT(msg.numPayloadBytes(), Eq(4));
  msg.rawPayloadBytes()[0] = 0xA0;
  msg.rawPayloadBytes()[1] = 0xA1;
  msg.rawPayloadBytes()[2] = 0xA2;
  msg.rawPayloadBytes()[3] = 0xA3;

  EXPECT_THAT(msg.numMsgBytesIncludingEox(), Eq(7));
  EXPECT_THAT(msg.rawMsgBytes()[0], Eq(0xF0));  // SysEx status byte.
  EXPECT_THAT(msg.rawMsgBytes()[1], Eq(0x01));  // SysEx ID (short manufacturer ID).

  EXPECT_THAT(msg.rawMsgBytes()[2], Eq(0xA0));  // Payload [0]
  EXPECT_THAT(msg.rawMsgBytes()[3], Eq(0xA1));  // Payload [1]
  EXPECT_THAT(msg.rawMsgBytes()[4], Eq(0xA2));  // Payload [2]
  EXPECT_THAT(msg.rawMsgBytes()[5], Eq(0xA3));  // Payload [3]

  EXPECT_THAT(msg.rawMsgBytes()[6], Eq(0xF7));  // EOX.

  // Test read-only access through const reference:
  const auto& msgRef = msg;
  EXPECT_THAT(msgRef.manufacturer(), Eq(kSeqCircuits));

  EXPECT_THAT(msgRef.numPayloadBytes(), Eq(4));
  EXPECT_THAT(msgRef.rawPayloadBytes()[0], Eq(0xA0));
  EXPECT_THAT(msgRef.rawPayloadBytes()[1], Eq(0xA1));
  EXPECT_THAT(msgRef.rawPayloadBytes()[2], Eq(0xA2));
  EXPECT_THAT(msgRef.rawPayloadBytes()[3], Eq(0xA3));

  EXPECT_THAT(msgRef.numMsgBytesIncludingEox(), Eq(7));
  EXPECT_THAT(msgRef.rawMsgBytes()[0], Eq(0xF0));
  EXPECT_THAT(msgRef.rawMsgBytes()[1], Eq(0x01));
  EXPECT_THAT(msgRef.rawMsgBytes()[2], Eq(0xA0));
  EXPECT_THAT(msgRef.rawMsgBytes()[3], Eq(0xA1));
  EXPECT_THAT(msgRef.rawMsgBytes()[4], Eq(0xA2));
  EXPECT_THAT(msgRef.rawMsgBytes()[5], Eq(0xA3));
  EXPECT_THAT(msgRef.rawMsgBytes()[6], Eq(0xF7));
}

TEST(MfrSysEx, WorksForNonCommercialIdWithinBuffer) {
  auto builder = bmmidi::MfrSysExBuilder{kNonCommercial}.withNumPayloadBytes(3);
  EXPECT_THAT(builder.numMsgBytesIncludingEox(), Eq(6));

  // Have MfrSysEx message reference into buffer[3]..buffer[8].
  std::uint8_t buffer[20] = {};

  auto msg = builder.buildAsRefToBytes(&buffer[3], 6);
  EXPECT_THAT(msg.manufacturer(), Eq(kNonCommercial));

  // Can fill in 3 payload bytes (with some fake payload data).
  EXPECT_THAT(msg.numPayloadBytes(), Eq(3));
  msg.rawPayloadBytes()[0] = 0xA0;
  msg.rawPayloadBytes()[1] = 0xA1;
  msg.rawPayloadBytes()[2] = 0xA2;

  EXPECT_THAT(msg.numMsgBytesIncludingEox(), Eq(6));
  EXPECT_THAT(msg.rawMsgBytes(), Eq(&buffer[3]));

  EXPECT_THAT(buffer[3], Eq(0xF0));  // SysEx status byte.
  EXPECT_THAT(buffer[4], Eq(0x7D));  // SysEx ID (non-commercial manufacturer ID).

  EXPECT_THAT(buffer[5], Eq(0xA0));  // Payload [0]
  EXPECT_THAT(buffer[6], Eq(0xA1));  // Payload [1]
  EXPECT_THAT(buffer[7], Eq(0xA2));  // Payload [2]

  EXPECT_THAT(buffer[8], Eq(0xF7));  // EOX.
}

TEST(MfrSysEx, WorksForExtIdOnHeap) {
  auto builder = bmmidi::MfrSysExBuilder{kSpectrasonics}.withNumPayloadBytes(4);
  EXPECT_THAT(builder.numMsgBytesIncludingEox(), Eq(9));

  auto msg = builder.buildOnHeap();
  EXPECT_THAT(msg.manufacturer(), Eq(kSpectrasonics));

  // Can fill in 4 payload bytes (with some fake payload data).
  EXPECT_THAT(msg.numPayloadBytes(), Eq(4));
  msg.rawPayloadBytes()[0] = 0xA0;
  msg.rawPayloadBytes()[1] = 0xA1;
  msg.rawPayloadBytes()[2] = 0xA2;
  msg.rawPayloadBytes()[3] = 0xA3;

  EXPECT_THAT(msg.numMsgBytesIncludingEox(), Eq(9));
  EXPECT_THAT(msg.rawMsgBytes()[0], Eq(0xF0));  // SysEx status byte.
  EXPECT_THAT(msg.rawMsgBytes()[1], Eq(0x00));  // SysEx ID (extended manufacturer ID).
  EXPECT_THAT(msg.rawMsgBytes()[2], Eq(0x02));  // Extended Manufacturer ID byte #1.
  EXPECT_THAT(msg.rawMsgBytes()[3], Eq(0x2C));  // Extended Manufacturer ID byte #2.

  EXPECT_THAT(msg.rawMsgBytes()[4], Eq(0xA0));  // Payload [0]
  EXPECT_THAT(msg.rawMsgBytes()[5], Eq(0xA1));  // Payload [1]
  EXPECT_THAT(msg.rawMsgBytes()[6], Eq(0xA2));  // Payload [2]
  EXPECT_THAT(msg.rawMsgBytes()[7], Eq(0xA3));  // Payload [3]

  EXPECT_THAT(msg.rawMsgBytes()[8], Eq(0xF7));  // EOX.
}

TEST(UniversalSysEx, WorksForOneSubIdTypeWithinBuffer) {
  const auto type = bmmidi::universal::kAck;
  const auto allDevices = bmmidi::Device::all();

  auto builder = bmmidi::UniversalSysExBuilder{type, allDevices}.withNumPayloadBytes(1);
  EXPECT_THAT(builder.numMsgBytesIncludingEox(), Eq(6));

  // Have UniversalSysEx message reference into buffer[3]..buffer[8].
  std::uint8_t buffer[20] = {};

  auto msg = builder.buildAsRefToBytes(&buffer[3], 6);
  EXPECT_THAT(msg.type(), Eq(bmmidi::universal::kAck));
  EXPECT_THAT(msg.device(), Eq(bmmidi::Device::all()));

  // Can fill in 1 payload byte (here the packet #).
  EXPECT_THAT(msg.numPayloadBytes(), Eq(1));
  msg.rawPayloadBytes()[0] = 0x3D;

  EXPECT_THAT(msg.numMsgBytesIncludingEox(), Eq(6));
  EXPECT_THAT(msg.rawMsgBytes(), Eq(&buffer[3]));

  EXPECT_THAT(buffer[3], Eq(0xF0));  // SysEx status byte.
  EXPECT_THAT(buffer[4], Eq(0x7E));  // SysEx ID (non-realtime universal).
  EXPECT_THAT(buffer[5], Eq(0x7F));  // Device ID ("all").
  EXPECT_THAT(buffer[6], Eq(0x7F));  // Sub ID #1 (0x7F).

  EXPECT_THAT(buffer[7], Eq(0x3D));  // Payload [0] (packet #).

  EXPECT_THAT(buffer[8], Eq(0xF7));  // EOX.

  // Test read-only access through const reference:
  const auto& msgRef = msg;
  EXPECT_THAT(msgRef.type(), Eq(bmmidi::universal::kAck));
  EXPECT_THAT(msgRef.device(), Eq(bmmidi::Device::all()));

  EXPECT_THAT(msgRef.numPayloadBytes(), Eq(1));
  EXPECT_THAT(msgRef.rawPayloadBytes()[0], Eq(0x3D));

  EXPECT_THAT(msgRef.numMsgBytesIncludingEox(), Eq(6));
  EXPECT_THAT(msgRef.rawMsgBytes()[0], Eq(0xF0));
  EXPECT_THAT(msgRef.rawMsgBytes()[1], Eq(0x7E));
  EXPECT_THAT(msgRef.rawMsgBytes()[2], Eq(0x7F));
  EXPECT_THAT(msgRef.rawMsgBytes()[3], Eq(0x7F));
  EXPECT_THAT(msgRef.rawMsgBytes()[4], Eq(0x3D));
  EXPECT_THAT(msgRef.rawMsgBytes()[5], Eq(0xF7));
}

TEST(UniversalSysEx, WorksForTwoSubIdTypeOnHeap) {
  const auto type = bmmidi::universal::kTuningBulkDumpReq;
  const auto device22 = bmmidi::Device::id(22);  // 0x16.

  auto builder = bmmidi::UniversalSysExBuilder{type, device22}.withNumPayloadBytes(1);
  EXPECT_THAT(builder.numMsgBytesIncludingEox(), Eq(7));

  auto msg = builder.buildOnHeap();
  EXPECT_THAT(msg.type(), Eq(bmmidi::universal::kTuningBulkDumpReq));
  EXPECT_THAT(msg.device().value(), Eq(22));

  // Can fill in the 1 payload byte (here the tuning program # being requested).
  EXPECT_THAT(msg.numPayloadBytes(), Eq(1));
  msg.rawPayloadBytes()[0] = 0x04;

  EXPECT_THAT(msg.numMsgBytesIncludingEox(), Eq(7));
  EXPECT_THAT(msg.rawMsgBytes()[0], Eq(0xF0));  // SysEx status byte.
  EXPECT_THAT(msg.rawMsgBytes()[1], Eq(0x7E));  // SysEx ID (non-realtime universal).
  EXPECT_THAT(msg.rawMsgBytes()[2], Eq(0x16));  // Device ID.
  EXPECT_THAT(msg.rawMsgBytes()[3], Eq(0x08));  // Sub-ID #1 (0x08).
  EXPECT_THAT(msg.rawMsgBytes()[4], Eq(0x00));  // Sub-ID #2 (0x00).

  EXPECT_THAT(msg.rawMsgBytes()[5], Eq(0x04));  // Payload [0] (tuning program #).

  EXPECT_THAT(msg.rawMsgBytes()[6], Eq(0xF7));  // EOX.
}

}  // namespace
