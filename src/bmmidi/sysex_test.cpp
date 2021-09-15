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

}  // namespace
