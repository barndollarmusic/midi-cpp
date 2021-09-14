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

}  // namespace
