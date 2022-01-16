// SPDX-FileCopyrightText: 2022 Barndollar Music, Ltd.
//
// SPDX-License-Identifier: Apache-2.0

#include "bmmidi/key_number.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;

TEST(KeyNumber, ProvidesConstantForNumKeys) {
  EXPECT_THAT(bmmidi::kNumKeys, Eq(128));
}

TEST(KeyNumber, ShouldWorkForAllNormalKeys) {
  // Should work for values 0 through 127.
  for (int keyNumber = 0; keyNumber <= 127; ++keyNumber) {
    const auto ch = bmmidi::KeyNumber::key(keyNumber);
    EXPECT_THAT(ch.isNormal(), IsTrue());
    EXPECT_THAT(ch.isNone(), IsFalse());
    EXPECT_THAT(ch.isAll(), IsFalse());
    EXPECT_THAT(ch.value(), Eq(keyNumber));
  }
}

TEST(KeyNumber, ProvidesUsefulKeys) {
  // Should provide first() and last() for limits.
  const auto minKey = bmmidi::KeyNumber::first();
  EXPECT_THAT(minKey.isNormal(), IsTrue());
  EXPECT_THAT(minKey.isNone(), IsFalse());
  EXPECT_THAT(minKey.isAll(), IsFalse());
  EXPECT_THAT(minKey.value(), Eq(0));

  const auto maxKey = bmmidi::KeyNumber::last();
  EXPECT_THAT(maxKey.isNormal(), IsTrue());
  EXPECT_THAT(maxKey.isNone(), IsFalse());
  EXPECT_THAT(maxKey.isAll(), IsFalse());
  EXPECT_THAT(maxKey.value(), Eq(127));

  // Should provide middleC() and tuningA() as common reference keys.
  const auto c4 = bmmidi::KeyNumber::middleC();
  EXPECT_THAT(c4.isNormal(), IsTrue());
  EXPECT_THAT(c4.isNone(), IsFalse());
  EXPECT_THAT(c4.isAll(), IsFalse());
  EXPECT_THAT(c4.value(), Eq(60));

  const auto a4 = bmmidi::KeyNumber::tuningA();
  EXPECT_THAT(a4.isNormal(), IsTrue());
  EXPECT_THAT(a4.isNone(), IsFalse());
  EXPECT_THAT(a4.isAll(), IsFalse());
  EXPECT_THAT(a4.value(), Eq(69));
}

TEST(KeyNumber, ShouldSupportSpecialValues) {
  // None: represents no key.
  const auto noKey = bmmidi::KeyNumber::none();
  EXPECT_THAT(noKey.isNormal(), IsFalse());
  EXPECT_THAT(noKey.isNone(), IsTrue());
  EXPECT_THAT(noKey.isAll(), IsFalse());

  // All: represents all keys.
  const auto omniKey = bmmidi::KeyNumber::all();
  EXPECT_THAT(omniKey.isNormal(), IsFalse());
  EXPECT_THAT(omniKey.isNone(), IsFalse());
  EXPECT_THAT(omniKey.isAll(), IsTrue());

  // Note: calling value() should fail assertions.
}

TEST(KeyNumber, ShouldSupportComparisons) {
  // Normal keys should compare by value.
  EXPECT_THAT(bmmidi::KeyNumber::key(0) == bmmidi::KeyNumber::key(0), IsTrue());
  EXPECT_THAT(bmmidi::KeyNumber::key(0) != bmmidi::KeyNumber::key(0), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::key(0) < bmmidi::KeyNumber::key(0), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::key(0) <= bmmidi::KeyNumber::key(0), IsTrue());
  EXPECT_THAT(bmmidi::KeyNumber::key(0) > bmmidi::KeyNumber::key(0), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::key(0) >= bmmidi::KeyNumber::key(0), IsTrue());

  EXPECT_THAT(bmmidi::KeyNumber::key(0) == bmmidi::KeyNumber::key(1), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::key(0) != bmmidi::KeyNumber::key(1), IsTrue());
  EXPECT_THAT(bmmidi::KeyNumber::key(0) < bmmidi::KeyNumber::key(1), IsTrue());
  EXPECT_THAT(bmmidi::KeyNumber::key(0) <= bmmidi::KeyNumber::key(1), IsTrue());
  EXPECT_THAT(bmmidi::KeyNumber::key(0) > bmmidi::KeyNumber::key(1), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::key(0) >= bmmidi::KeyNumber::key(1), IsFalse());

  EXPECT_THAT(bmmidi::KeyNumber::key(127) == bmmidi::KeyNumber::key(0), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::key(127) != bmmidi::KeyNumber::key(0), IsTrue());
  EXPECT_THAT(bmmidi::KeyNumber::key(127) < bmmidi::KeyNumber::key(0), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::key(127) <= bmmidi::KeyNumber::key(0), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::key(127) > bmmidi::KeyNumber::key(0), IsTrue());
  EXPECT_THAT(bmmidi::KeyNumber::key(127) >= bmmidi::KeyNumber::key(0), IsTrue());

  // None should compare greater than other normal keys.
  EXPECT_THAT(bmmidi::KeyNumber::none() < bmmidi::KeyNumber::key(127), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::none() > bmmidi::KeyNumber::key(127), IsTrue());

  // All should compare greater than other keys and None.
  EXPECT_THAT(bmmidi::KeyNumber::all() < bmmidi::KeyNumber::none(), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::all() > bmmidi::KeyNumber::none(), IsTrue());
  EXPECT_THAT(bmmidi::KeyNumber::none() < bmmidi::KeyNumber::all(), IsTrue());
  EXPECT_THAT(bmmidi::KeyNumber::none() > bmmidi::KeyNumber::all(), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::all() < bmmidi::KeyNumber::key(127), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::all() > bmmidi::KeyNumber::key(127), IsTrue());

  // Both special values should equal themselves.
  EXPECT_THAT(bmmidi::KeyNumber::none() == bmmidi::KeyNumber::none(), IsTrue());
  EXPECT_THAT(bmmidi::KeyNumber::none() != bmmidi::KeyNumber::none(), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::all() == bmmidi::KeyNumber::all(), IsTrue());
  EXPECT_THAT(bmmidi::KeyNumber::all() != bmmidi::KeyNumber::all(), IsFalse());
}

TEST(KeyNumber, SupportsIncrDecr) {
  auto keyA = bmmidi::KeyNumber::first();
  EXPECT_THAT(keyA.value(), Eq(0));

  // Pre-increment:
  EXPECT_THAT((++keyA).value(), Eq(1));
  EXPECT_THAT(keyA.value(), Eq(1));

  // Post-increment:
  EXPECT_THAT((keyA++).value(), Eq(1));
  EXPECT_THAT(keyA.value(), Eq(2));

  auto keyB = bmmidi::KeyNumber::last();
  EXPECT_THAT(keyB.value(), Eq(127));

  // Pre-decrement:
  EXPECT_THAT((--keyB).value(), Eq(126));
  EXPECT_THAT(keyB.value(), Eq(126));

  // Post-decrement:
  EXPECT_THAT((keyB--).value(), Eq(126));
  EXPECT_THAT(keyB.value(), Eq(125));

  // Supports decrement down to KeyNumber::first():
  EXPECT_THAT(--bmmidi::KeyNumber::key(1), Eq(bmmidi::KeyNumber::first()));

  // KeyNumber::none() represents one-beyond-the-last:
  EXPECT_THAT(++bmmidi::KeyNumber::last(), Eq(bmmidi::KeyNumber::none()));
  EXPECT_THAT(--bmmidi::KeyNumber::none(), Eq(bmmidi::KeyNumber::last()));

  // NOTE: Incrementing or decrementing KeyNumber::all() fails debug assertions,
  // since it would result in an invalid value.
}

}  // namespace
