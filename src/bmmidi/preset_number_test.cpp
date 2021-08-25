#include "bmmidi/preset_number.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;

TEST(PresetNumber, ShouldWorkForAllNormalPresets) {
  // Should work for index 0 through 127.
  for (int presetIndex = 0; presetIndex <= 127; ++presetIndex) {
    const auto preset = bmmidi::PresetNumber::index(presetIndex);
    EXPECT_THAT(preset.isNormal(), IsTrue());
    EXPECT_THAT(preset.isNone(), IsFalse());
    EXPECT_THAT(preset.index(), Eq(presetIndex));
    EXPECT_THAT(preset.displayNumber(), Eq(presetIndex + 1));
  }

  // Should also provide first() and last() for limits.
  const auto minPreset = bmmidi::PresetNumber::first();
  EXPECT_THAT(minPreset.isNormal(), IsTrue());
  EXPECT_THAT(minPreset.isNone(), IsFalse());
  EXPECT_THAT(minPreset.index(), Eq(0));
  EXPECT_THAT(minPreset.displayNumber(), Eq(1));

  const auto maxPreset = bmmidi::PresetNumber::last();
  EXPECT_THAT(maxPreset.isNormal(), IsTrue());
  EXPECT_THAT(maxPreset.isNone(), IsFalse());
  EXPECT_THAT(maxPreset.index(), Eq(127));
  EXPECT_THAT(maxPreset.displayNumber(), Eq(128));
}

TEST(PresetNumber, ShouldSupportSpecialValues) {
  // None: represents no selected preset.
  const auto noPreset = bmmidi::PresetNumber::none();
  EXPECT_THAT(noPreset.isNormal(), IsFalse());
  EXPECT_THAT(noPreset.isNone(), IsTrue());

  // Note: calling index() or displayNumber() should fail assertions.
}

TEST(PresetNumber, ShouldSupportComparisons) {
  // Normal presets should compare by index.
  EXPECT_THAT(bmmidi::PresetNumber::index(0) == bmmidi::PresetNumber::index(0), IsTrue());
  EXPECT_THAT(bmmidi::PresetNumber::index(0) != bmmidi::PresetNumber::index(0), IsFalse());
  EXPECT_THAT(bmmidi::PresetNumber::index(0) < bmmidi::PresetNumber::index(0), IsFalse());
  EXPECT_THAT(bmmidi::PresetNumber::index(0) <= bmmidi::PresetNumber::index(0), IsTrue());
  EXPECT_THAT(bmmidi::PresetNumber::index(0) > bmmidi::PresetNumber::index(0), IsFalse());
  EXPECT_THAT(bmmidi::PresetNumber::index(0) >= bmmidi::PresetNumber::index(0), IsTrue());

  EXPECT_THAT(bmmidi::PresetNumber::index(0) == bmmidi::PresetNumber::index(1), IsFalse());
  EXPECT_THAT(bmmidi::PresetNumber::index(0) != bmmidi::PresetNumber::index(1), IsTrue());
  EXPECT_THAT(bmmidi::PresetNumber::index(0) < bmmidi::PresetNumber::index(1), IsTrue());
  EXPECT_THAT(bmmidi::PresetNumber::index(0) <= bmmidi::PresetNumber::index(1), IsTrue());
  EXPECT_THAT(bmmidi::PresetNumber::index(0) > bmmidi::PresetNumber::index(1), IsFalse());
  EXPECT_THAT(bmmidi::PresetNumber::index(0) >= bmmidi::PresetNumber::index(1), IsFalse());

  EXPECT_THAT(bmmidi::PresetNumber::index(127) == bmmidi::PresetNumber::index(0), IsFalse());
  EXPECT_THAT(bmmidi::PresetNumber::index(127) != bmmidi::PresetNumber::index(0), IsTrue());
  EXPECT_THAT(bmmidi::PresetNumber::index(127) < bmmidi::PresetNumber::index(0), IsFalse());
  EXPECT_THAT(bmmidi::PresetNumber::index(127) <= bmmidi::PresetNumber::index(0), IsFalse());
  EXPECT_THAT(bmmidi::PresetNumber::index(127) > bmmidi::PresetNumber::index(0), IsTrue());
  EXPECT_THAT(bmmidi::PresetNumber::index(127) >= bmmidi::PresetNumber::index(0), IsTrue());

  // None should compare less than other presets.
  EXPECT_THAT(bmmidi::PresetNumber::none() < bmmidi::PresetNumber::index(0), IsTrue());
  EXPECT_THAT(bmmidi::PresetNumber::none() > bmmidi::PresetNumber::index(0), IsFalse());
  EXPECT_THAT(bmmidi::PresetNumber::none() < bmmidi::PresetNumber::index(64), IsTrue());
  EXPECT_THAT(bmmidi::PresetNumber::none() > bmmidi::PresetNumber::index(64), IsFalse());

  // None should equal itself.
  EXPECT_THAT(bmmidi::PresetNumber::none() == bmmidi::PresetNumber::none(), IsTrue());
  EXPECT_THAT(bmmidi::PresetNumber::none() != bmmidi::PresetNumber::none(), IsFalse());
}

}  // namespace
