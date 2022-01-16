// SPDX-FileCopyrightText: 2022 Barndollar Music, Ltd.
//
// SPDX-License-Identifier: Apache-2.0

#include "bmmidi/bitops.hpp"

#include <cstdint>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace bmmidi {
namespace {

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;

TEST(GetBits, ReturnsOneBitsInMask) {
  EXPECT_THAT(internal::getBits(0b0110'1011,
                                0b0010'0111),
                             Eq(0b0010'0011));

  EXPECT_THAT(internal::getBits(0b0101'1010,
                                0b0000'0000),
                             Eq(0b0000'0000));

  EXPECT_THAT(internal::getBits(0b0101'1010,
                                0b1111'1111),
                             Eq(0b0101'1010));

  EXPECT_THAT(internal::getBits(0b1101'0110,
                                0b1111'0000),
                             Eq(0b1101'0000));

  EXPECT_THAT(internal::getBits(0b1101'0110,
                                0b0011'1111),
                             Eq(0b0001'0110));
}

TEST(SetBits, ReplacesOnlyOneBitsInMask) {
  EXPECT_THAT(internal::setBits(0b0110'1011,
                                0b1111'0110,
                                0b0000'1111),
                             Eq(0b0110'0110));

  EXPECT_THAT(internal::setBits(0b0110'1011,
                                0b1111'0110,
                                0b0101'0101),
                             Eq(0b0111'1110));

  EXPECT_THAT(internal::setBits(0b0110'1011,
                                0b0000'1001,
                                0b0000'1111),
                             Eq(0b0110'1001));

  EXPECT_THAT(internal::setBits(0b0101'1101,
                                0b0010'0000,
                                0b0110'0000),
                             Eq(0b0011'1101));
}

}  // namespace
}  // bmmidi
