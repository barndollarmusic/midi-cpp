// SPDX-FileCopyrightText: 2022 Barndollar Music, Ltd.
//
// SPDX-License-Identifier: Apache-2.0

#include "bmmidi/preset_number.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;

TEST(PresetNumber, ProvidesNumPresetsConstant) {
  EXPECT_THAT(bmmidi::kNumPresets, Eq(128));
}

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

  // None should compare greater than other normal presets.
  EXPECT_THAT(bmmidi::PresetNumber::none() < bmmidi::PresetNumber::index(0), IsFalse());
  EXPECT_THAT(bmmidi::PresetNumber::none() > bmmidi::PresetNumber::index(0), IsTrue());
  EXPECT_THAT(bmmidi::PresetNumber::none() < bmmidi::PresetNumber::index(64), IsFalse());
  EXPECT_THAT(bmmidi::PresetNumber::none() > bmmidi::PresetNumber::index(64), IsTrue());

  // None should equal itself.
  EXPECT_THAT(bmmidi::PresetNumber::none() == bmmidi::PresetNumber::none(), IsTrue());
  EXPECT_THAT(bmmidi::PresetNumber::none() != bmmidi::PresetNumber::none(), IsFalse());
}

TEST(PresetNumber, SupportsIncrDecr) {
  auto presetA = bmmidi::PresetNumber::first();
  EXPECT_THAT(presetA.index(), Eq(0));

  // Pre-increment:
  EXPECT_THAT((++presetA).index(), Eq(1));
  EXPECT_THAT(presetA.index(), Eq(1));

  // Post-increment:
  EXPECT_THAT((presetA++).index(), Eq(1));
  EXPECT_THAT(presetA.index(), Eq(2));

  auto presetB = bmmidi::PresetNumber::last();
  EXPECT_THAT(presetB.index(), Eq(127));

  // Pre-decrement:
  EXPECT_THAT((--presetB).index(), Eq(126));
  EXPECT_THAT(presetB.index(), Eq(126));

  // Post-decrement:
  EXPECT_THAT((presetB--).index(), Eq(126));
  EXPECT_THAT(presetB.index(), Eq(125));

  // Supports decrement down to PresetNumber::first():
  EXPECT_THAT(--bmmidi::PresetNumber::index(1), Eq(bmmidi::PresetNumber::first()));

  // PresetNumber::none() represents one-beyond-the-last:
  EXPECT_THAT(++bmmidi::PresetNumber::last(), Eq(bmmidi::PresetNumber::none()));
  EXPECT_THAT(--bmmidi::PresetNumber::none(), Eq(bmmidi::PresetNumber::last()));
}

}  // namespace
