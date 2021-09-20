#include "bmmidi/data_value.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;

constexpr double kPrecision8 = 0.00000001;

inline auto DoubleEq(double expected) {
  return ::testing::DoubleNear(expected, kPrecision8);
}

TEST(DataValue, ProvidesNumDataValuesConstant) {
  EXPECT_THAT(bmmidi::kNumDataValues, Eq(128));
}

TEST(DataValue, ShouldSupportAllValues) {
  // Should work for values 0 through 127.
  {
    const auto v0 = bmmidi::DataValue{0};
    EXPECT_THAT(v0.value(), Eq(0));
    EXPECT_THAT(v0.toNormalized0To1<float>(), Eq(0.0f));
    EXPECT_THAT(v0.toNormalized0To1<double>(), Eq(0.0));
    EXPECT_THAT(v0.toNormalized0To1<long double>(), Eq(0.0L));
  }

  {
    const auto v1 = bmmidi::DataValue{1};
    EXPECT_THAT(v1.value(), Eq(1));
    EXPECT_THAT(v1.toNormalized0To1<double>(), DoubleEq(0.00787402));
  }

  {
    const auto v47 = bmmidi::DataValue{47};
    EXPECT_THAT(v47.value(), Eq(47));
    EXPECT_THAT(v47.toNormalized0To1<double>(), DoubleEq(0.37007874));
  }

  {
    const auto v64 = bmmidi::DataValue{64};
    EXPECT_THAT(v64.value(), Eq(64));
    EXPECT_THAT(v64.toNormalized0To1<double>(), DoubleEq(0.50393701));
  }

  {
    const auto v108 = bmmidi::DataValue{108};
    EXPECT_THAT(v108.value(), Eq(108));
    EXPECT_THAT(v108.toNormalized0To1<double>(), DoubleEq(0.85039370));
  }

  {
    const auto v127 = bmmidi::DataValue{127};
    EXPECT_THAT(v127.value(), Eq(127));
    EXPECT_THAT(v127.toNormalized0To1<float>(), Eq(1.0f));
    EXPECT_THAT(v127.toNormalized0To1<double>(), Eq(1.0));
    EXPECT_THAT(v127.toNormalized0To1<long double>(), Eq(1.0L));
  }

  // Should provide min(), midpoint(), and max() for convenience.
  EXPECT_THAT(bmmidi::DataValue::min().value(), Eq(0));
  EXPECT_THAT(bmmidi::DataValue::midpoint().value(), Eq(64));
  EXPECT_THAT(bmmidi::DataValue::max().value(), Eq(127));
}

TEST(DataValue, ShouldConvertFromNormalized) {
  EXPECT_THAT(bmmidi::DataValue::fromNormalized0To1(0.0).value(), Eq(0));
  EXPECT_THAT(bmmidi::DataValue::fromNormalized0To1(0.008).value(), Eq(1));
  EXPECT_THAT(bmmidi::DataValue::fromNormalized0To1(0.37).value(), Eq(47));
  EXPECT_THAT(bmmidi::DataValue::fromNormalized0To1(0.5).value(), Eq(64));
  EXPECT_THAT(bmmidi::DataValue::fromNormalized0To1(0.85).value(), Eq(108));
  EXPECT_THAT(bmmidi::DataValue::fromNormalized0To1(1.0).value(), Eq(127));
}

TEST(DataValue, ShouldSupportComparisons) {
  EXPECT_THAT(bmmidi::DataValue{0} == bmmidi::DataValue{0}, IsTrue());
  EXPECT_THAT(bmmidi::DataValue{0} != bmmidi::DataValue{0}, IsFalse());
  EXPECT_THAT(bmmidi::DataValue{0} < bmmidi::DataValue{0}, IsFalse());
  EXPECT_THAT(bmmidi::DataValue{0} <= bmmidi::DataValue{0}, IsTrue());
  EXPECT_THAT(bmmidi::DataValue{0} > bmmidi::DataValue{0}, IsFalse());
  EXPECT_THAT(bmmidi::DataValue{0} >= bmmidi::DataValue{0}, IsTrue());

  EXPECT_THAT(bmmidi::DataValue{0} == bmmidi::DataValue{1}, IsFalse());
  EXPECT_THAT(bmmidi::DataValue{0} != bmmidi::DataValue{1}, IsTrue());
  EXPECT_THAT(bmmidi::DataValue{0} < bmmidi::DataValue{1}, IsTrue());
  EXPECT_THAT(bmmidi::DataValue{0} <= bmmidi::DataValue{1}, IsTrue());
  EXPECT_THAT(bmmidi::DataValue{0} > bmmidi::DataValue{1}, IsFalse());
  EXPECT_THAT(bmmidi::DataValue{0} >= bmmidi::DataValue{1}, IsFalse());

  EXPECT_THAT(bmmidi::DataValue{127} == bmmidi::DataValue{0}, IsFalse());
  EXPECT_THAT(bmmidi::DataValue{127} != bmmidi::DataValue{0}, IsTrue());
  EXPECT_THAT(bmmidi::DataValue{127} < bmmidi::DataValue{0}, IsFalse());
  EXPECT_THAT(bmmidi::DataValue{127} <= bmmidi::DataValue{0}, IsFalse());
  EXPECT_THAT(bmmidi::DataValue{127} > bmmidi::DataValue{0}, IsTrue());
  EXPECT_THAT(bmmidi::DataValue{127} >= bmmidi::DataValue{0}, IsTrue());
}

TEST(DoubleDataValue, ProvidesNumDoubleDataValuesConstant) {
  EXPECT_THAT(bmmidi::kNumDoubleDataValues, Eq(16384));
}

TEST(DoubleDataValue, ShouldSupportAllValues) {
  // Should work for values 0 through 16383.
  {
    const auto v0 = bmmidi::DoubleDataValue{0};
    EXPECT_THAT(v0.value(), Eq(0));
  }

  {
    const auto v1 = bmmidi::DoubleDataValue{1};
    EXPECT_THAT(v1.value(), Eq(1));
  }

  {
    const auto v3427 = bmmidi::DoubleDataValue{3427};
    EXPECT_THAT(v3427.value(), Eq(3427));
  }

  {
    const auto v8192 = bmmidi::DoubleDataValue{8192};
    EXPECT_THAT(v8192.value(), Eq(8192));
  }

  {
    const auto v15243 = bmmidi::DoubleDataValue{15243};
    EXPECT_THAT(v15243.value(), Eq(15243));
  }

  {
    const auto v16383 = bmmidi::DoubleDataValue{16383};
    EXPECT_THAT(v16383.value(), Eq(16383));
  }

  // Should provide min(), midpoint(), and max() for convenience.
  EXPECT_THAT(bmmidi::DoubleDataValue::min().value(), Eq(0));
  EXPECT_THAT(bmmidi::DoubleDataValue::midpoint().value(), Eq(8192));
  EXPECT_THAT(bmmidi::DoubleDataValue::max().value(), Eq(16383));
}

TEST(DoubleDataValue, ShouldConvertToFromLsbMsb) {
  {
    // LSB:            0b000 0000 (  0x00 = 0)
    // MSB:   0b00 0000 0         (  0x00 = 0)
    // Val: 0b  00 0000 0000 0000 (0x0000 = 0)
    const auto v0 = bmmidi::DoubleDataValue::fromLsbMsb(0x00, 0x00);
    EXPECT_THAT(v0.value(), Eq(0x0000));
    EXPECT_THAT(v0.lsb(), Eq(0x00));
    EXPECT_THAT(v0.msb(), Eq(0x00));
  }

  {
    // LSB:            0b000 0001 (  0x01 = 1)
    // MSB:   0b00 0000 0         (  0x00 = 0)
    // Val: 0b  00 0000 0000 0001 (0x0001 = 1)
    const auto v1 = bmmidi::DoubleDataValue::fromLsbMsb(0x01, 0x00);
    EXPECT_THAT(v1.value(), Eq(0x0001));
    EXPECT_THAT(v1.lsb(), Eq(0x01));
    EXPECT_THAT(v1.msb(), Eq(0x00));
  }

  {
    // LSB:            0b110 0011 (  0x63 = 99)
    // MSB:   0b00 1101 0         (  0x1A = 26)
    // Val: 0b  00 1101 0110 0011 (0x0D63 = 3427)
    const auto v3427 = bmmidi::DoubleDataValue::fromLsbMsb(0x63, 0x1A);
    EXPECT_THAT(v3427.value(), Eq(0x0D63));
    EXPECT_THAT(v3427.lsb(), Eq(0x63));
    EXPECT_THAT(v3427.msb(), Eq(0x1A));
  }

  {
    // LSB:            0b000 0000 (  0x00 = 0)
    // MSB:   0b10 0000 0         (  0x40 = 64)
    // Val: 0b  10 0000 0000 0000 (0x2000 = 8192)
    const auto v8192 = bmmidi::DoubleDataValue::fromLsbMsb(0x00, 0x40);
    EXPECT_THAT(v8192.value(), Eq(0x2000));
    EXPECT_THAT(v8192.lsb(), Eq(0x00));
    EXPECT_THAT(v8192.msb(), Eq(0x40));
  }

  {
    // LSB:            0b000 1011 (  0x0B = 11)
    // MSB:   0b11 1011 1         (  0x77 = 119)
    // Val: 0b  11 1011 1000 1011 (0x3B8B = 15243)
    const auto v15243 = bmmidi::DoubleDataValue::fromLsbMsb(0x0B, 0x77);
    EXPECT_THAT(v15243.value(), Eq(0x3B8B));
    EXPECT_THAT(v15243.lsb(), Eq(0x0B));
    EXPECT_THAT(v15243.msb(), Eq(0x77));
  }

  {
    // LSB:            0b111 1111 (  0x7F = 127)
    // MSB:   0b11 1111 1         (  0x7F = 127)
    // Val: 0b  11 1111 1111 1111 (0x3FFF = 16383)
    const auto v16383 = bmmidi::DoubleDataValue::fromLsbMsb(0x7F, 0x7F);
    EXPECT_THAT(v16383.value(), Eq(0x3FFF));
    EXPECT_THAT(v16383.lsb(), Eq(0x7F));
    EXPECT_THAT(v16383.msb(), Eq(0x7F));
  }
}

TEST(DoubleDataValue, ShouldSupportComparisons) {
  EXPECT_THAT(bmmidi::DoubleDataValue{0} == bmmidi::DoubleDataValue{0}, IsTrue());
  EXPECT_THAT(bmmidi::DoubleDataValue{0} != bmmidi::DoubleDataValue{0}, IsFalse());
  EXPECT_THAT(bmmidi::DoubleDataValue{0} < bmmidi::DoubleDataValue{0}, IsFalse());
  EXPECT_THAT(bmmidi::DoubleDataValue{0} <= bmmidi::DoubleDataValue{0}, IsTrue());
  EXPECT_THAT(bmmidi::DoubleDataValue{0} > bmmidi::DoubleDataValue{0}, IsFalse());
  EXPECT_THAT(bmmidi::DoubleDataValue{0} >= bmmidi::DoubleDataValue{0}, IsTrue());

  EXPECT_THAT(bmmidi::DoubleDataValue{0} == bmmidi::DoubleDataValue{1}, IsFalse());
  EXPECT_THAT(bmmidi::DoubleDataValue{0} != bmmidi::DoubleDataValue{1}, IsTrue());
  EXPECT_THAT(bmmidi::DoubleDataValue{0} < bmmidi::DoubleDataValue{1}, IsTrue());
  EXPECT_THAT(bmmidi::DoubleDataValue{0} <= bmmidi::DoubleDataValue{1}, IsTrue());
  EXPECT_THAT(bmmidi::DoubleDataValue{0} > bmmidi::DoubleDataValue{1}, IsFalse());
  EXPECT_THAT(bmmidi::DoubleDataValue{0} >= bmmidi::DoubleDataValue{1}, IsFalse());

  EXPECT_THAT(bmmidi::DoubleDataValue{16383} == bmmidi::DoubleDataValue{0}, IsFalse());
  EXPECT_THAT(bmmidi::DoubleDataValue{16383} != bmmidi::DoubleDataValue{0}, IsTrue());
  EXPECT_THAT(bmmidi::DoubleDataValue{16383} < bmmidi::DoubleDataValue{0}, IsFalse());
  EXPECT_THAT(bmmidi::DoubleDataValue{16383} <= bmmidi::DoubleDataValue{0}, IsFalse());
  EXPECT_THAT(bmmidi::DoubleDataValue{16383} > bmmidi::DoubleDataValue{0}, IsTrue());
  EXPECT_THAT(bmmidi::DoubleDataValue{16383} >= bmmidi::DoubleDataValue{0}, IsTrue());
}

}  // namespace
