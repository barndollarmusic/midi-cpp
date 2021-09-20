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

  // Should also provide first() and last() for limits.
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

  // None should compare less than other keys.
  EXPECT_THAT(bmmidi::KeyNumber::none() < bmmidi::KeyNumber::key(0), IsTrue());
  EXPECT_THAT(bmmidi::KeyNumber::none() > bmmidi::KeyNumber::key(0), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::none() < bmmidi::KeyNumber::all(), IsTrue());
  EXPECT_THAT(bmmidi::KeyNumber::none() > bmmidi::KeyNumber::all(), IsFalse());

  // Omni should compare greater than other keys.
  EXPECT_THAT(bmmidi::KeyNumber::all() < bmmidi::KeyNumber::none(), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::all() > bmmidi::KeyNumber::none(), IsTrue());
  EXPECT_THAT(bmmidi::KeyNumber::all() < bmmidi::KeyNumber::key(127), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::all() > bmmidi::KeyNumber::key(127), IsTrue());

  // Both special values should equal themselves.
  EXPECT_THAT(bmmidi::KeyNumber::none() == bmmidi::KeyNumber::none(), IsTrue());
  EXPECT_THAT(bmmidi::KeyNumber::none() != bmmidi::KeyNumber::none(), IsFalse());
  EXPECT_THAT(bmmidi::KeyNumber::all() == bmmidi::KeyNumber::all(), IsTrue());
  EXPECT_THAT(bmmidi::KeyNumber::all() != bmmidi::KeyNumber::all(), IsFalse());
}

}  // namespace
