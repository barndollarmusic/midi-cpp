#include "bmmidi/channel.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;

TEST(Channel, ProvidesNumChannelsConstant) {
  EXPECT_THAT(bmmidi::kNumChannels, Eq(16));
}

TEST(Channel, ShouldWorkForAllNormalChannels) {
  // Should work for index 0 through 15.
  for (int chIndex = 0; chIndex <= 15; ++chIndex) {
    const auto ch = bmmidi::Channel::index(chIndex);
    EXPECT_THAT(ch.isNormal(), IsTrue());
    EXPECT_THAT(ch.isNone(), IsFalse());
    EXPECT_THAT(ch.isOmni(), IsFalse());
    EXPECT_THAT(ch.index(), Eq(chIndex));
    EXPECT_THAT(ch.displayNumber(), Eq(chIndex + 1));
  }

  // Should also provide first() and last() for limits.
  const auto minCh = bmmidi::Channel::first();
  EXPECT_THAT(minCh.isNormal(), IsTrue());
  EXPECT_THAT(minCh.isNone(), IsFalse());
  EXPECT_THAT(minCh.isOmni(), IsFalse());
  EXPECT_THAT(minCh.index(), Eq(0));
  EXPECT_THAT(minCh.displayNumber(), Eq(1));

  const auto maxCh = bmmidi::Channel::last();
  EXPECT_THAT(maxCh.isNormal(), IsTrue());
  EXPECT_THAT(maxCh.isNone(), IsFalse());
  EXPECT_THAT(maxCh.isOmni(), IsFalse());
  EXPECT_THAT(maxCh.index(), Eq(15));
  EXPECT_THAT(maxCh.displayNumber(), Eq(16));
}

TEST(Channel, ShouldSupportSpecialValues) {
  // None: represents no selected channel.
  const auto noCh = bmmidi::Channel::none();
  EXPECT_THAT(noCh.isNormal(), IsFalse());
  EXPECT_THAT(noCh.isNone(), IsTrue());
  EXPECT_THAT(noCh.isOmni(), IsFalse());

  // Omni: represents all channels.
  const auto omniCh = bmmidi::Channel::omni();
  EXPECT_THAT(omniCh.isNormal(), IsFalse());
  EXPECT_THAT(omniCh.isNone(), IsFalse());
  EXPECT_THAT(omniCh.isOmni(), IsTrue());

  // Note: calling index() or displayNumber() should fail assertions.
}

TEST(Channel, ShouldSupportComparisons) {
  // Normal channels should compare by index.
  EXPECT_THAT(bmmidi::Channel::index(0) == bmmidi::Channel::index(0), IsTrue());
  EXPECT_THAT(bmmidi::Channel::index(0) != bmmidi::Channel::index(0), IsFalse());
  EXPECT_THAT(bmmidi::Channel::index(0) < bmmidi::Channel::index(0), IsFalse());
  EXPECT_THAT(bmmidi::Channel::index(0) <= bmmidi::Channel::index(0), IsTrue());
  EXPECT_THAT(bmmidi::Channel::index(0) > bmmidi::Channel::index(0), IsFalse());
  EXPECT_THAT(bmmidi::Channel::index(0) >= bmmidi::Channel::index(0), IsTrue());

  EXPECT_THAT(bmmidi::Channel::index(0) == bmmidi::Channel::index(1), IsFalse());
  EXPECT_THAT(bmmidi::Channel::index(0) != bmmidi::Channel::index(1), IsTrue());
  EXPECT_THAT(bmmidi::Channel::index(0) < bmmidi::Channel::index(1), IsTrue());
  EXPECT_THAT(bmmidi::Channel::index(0) <= bmmidi::Channel::index(1), IsTrue());
  EXPECT_THAT(bmmidi::Channel::index(0) > bmmidi::Channel::index(1), IsFalse());
  EXPECT_THAT(bmmidi::Channel::index(0) >= bmmidi::Channel::index(1), IsFalse());

  EXPECT_THAT(bmmidi::Channel::index(15) == bmmidi::Channel::index(0), IsFalse());
  EXPECT_THAT(bmmidi::Channel::index(15) != bmmidi::Channel::index(0), IsTrue());
  EXPECT_THAT(bmmidi::Channel::index(15) < bmmidi::Channel::index(0), IsFalse());
  EXPECT_THAT(bmmidi::Channel::index(15) <= bmmidi::Channel::index(0), IsFalse());
  EXPECT_THAT(bmmidi::Channel::index(15) > bmmidi::Channel::index(0), IsTrue());
  EXPECT_THAT(bmmidi::Channel::index(15) >= bmmidi::Channel::index(0), IsTrue());

  // None should compare greater than other normal channels.
  EXPECT_THAT(bmmidi::Channel::none() < bmmidi::Channel::index(15), IsFalse());
  EXPECT_THAT(bmmidi::Channel::none() > bmmidi::Channel::index(15), IsTrue());

  // Omni should compare greater than other channels and None.
  EXPECT_THAT(bmmidi::Channel::omni() < bmmidi::Channel::none(), IsFalse());
  EXPECT_THAT(bmmidi::Channel::omni() > bmmidi::Channel::none(), IsTrue());
  EXPECT_THAT(bmmidi::Channel::none() < bmmidi::Channel::omni(), IsTrue());
  EXPECT_THAT(bmmidi::Channel::none() > bmmidi::Channel::omni(), IsFalse());

  EXPECT_THAT(bmmidi::Channel::omni() < bmmidi::Channel::index(15), IsFalse());
  EXPECT_THAT(bmmidi::Channel::omni() > bmmidi::Channel::index(15), IsTrue());

  // Both special values should equal themselves.
  EXPECT_THAT(bmmidi::Channel::none() == bmmidi::Channel::none(), IsTrue());
  EXPECT_THAT(bmmidi::Channel::none() != bmmidi::Channel::none(), IsFalse());
  EXPECT_THAT(bmmidi::Channel::omni() == bmmidi::Channel::omni(), IsTrue());
  EXPECT_THAT(bmmidi::Channel::omni() != bmmidi::Channel::omni(), IsFalse());
}

TEST(Channel, SupportsIncrDecr) {
  auto chanA = bmmidi::Channel::first();
  EXPECT_THAT(chanA.index(), Eq(0));

  // Pre-increment:
  EXPECT_THAT((++chanA).index(), Eq(1));
  EXPECT_THAT(chanA.index(), Eq(1));

  // Post-increment:
  EXPECT_THAT((chanA++).index(), Eq(1));
  EXPECT_THAT(chanA.index(), Eq(2));

  auto chanB = bmmidi::Channel::last();
  EXPECT_THAT(chanB.index(), Eq(15));

  // Pre-decrement:
  EXPECT_THAT((--chanB).index(), Eq(14));
  EXPECT_THAT(chanB.index(), Eq(14));

  // Post-decrement:
  EXPECT_THAT((chanB--).index(), Eq(14));
  EXPECT_THAT(chanB.index(), Eq(13));

  // Supports decrement down to Channel::first():
  EXPECT_THAT(--bmmidi::Channel::index(1), Eq(bmmidi::Channel::first()));

  // Channel::none() represents one-beyond-the-last:
  EXPECT_THAT(++bmmidi::Channel::last(), Eq(bmmidi::Channel::none()));
  EXPECT_THAT(--bmmidi::Channel::none(), Eq(bmmidi::Channel::last()));

  // NOTE: Incrementing or decrementing Channel::omni() fails debug assertions,
  // since it would result in an invalid value.
}

}  // namespace
