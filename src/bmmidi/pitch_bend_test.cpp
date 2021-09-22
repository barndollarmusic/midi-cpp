#include "bmmidi/pitch_bend.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "bmmidi/data_value.hpp"

namespace {

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;

constexpr double kNegInfinity = -std::numeric_limits<double>::infinity();
constexpr double kInfinity = std::numeric_limits<double>::infinity();

constexpr double kPrecision8 = 0.00000001;

inline auto DoubleEq(double expected) {
  return ::testing::DoubleNear(expected, kPrecision8);
}

TEST(PitchBendRange, ShouldDefaultToUpDown2) {
  EXPECT_THAT(bmmidi::PitchBendRange::defaultWholeStep().maxSemitonesDown(), Eq(2.0));
  EXPECT_THAT(bmmidi::PitchBendRange::defaultWholeStep().maxSemitonesUp(), Eq(2.0));
}

TEST(PitchBendRange, ShouldAllowSymmetricalRange) {
  EXPECT_THAT(bmmidi::PitchBendRange::symmetrical(0.0).maxSemitonesDown(), Eq(0.0));
  EXPECT_THAT(bmmidi::PitchBendRange::symmetrical(0.0).maxSemitonesUp(), Eq(0.0));

  EXPECT_THAT(bmmidi::PitchBendRange::symmetrical(0.5).maxSemitonesDown(), Eq(0.5));
  EXPECT_THAT(bmmidi::PitchBendRange::symmetrical(0.5).maxSemitonesUp(), Eq(0.5));

  EXPECT_THAT(bmmidi::PitchBendRange::symmetrical(1.234).maxSemitonesDown(), Eq(1.234));
  EXPECT_THAT(bmmidi::PitchBendRange::symmetrical(1.234).maxSemitonesUp(), Eq(1.234));

  EXPECT_THAT(bmmidi::PitchBendRange::symmetrical(12).maxSemitonesDown(), Eq(12.0));
  EXPECT_THAT(bmmidi::PitchBendRange::symmetrical(12).maxSemitonesUp(), Eq(12.0));
}

TEST(PitchBendRange, ShouldAllowAsymmetricalRange) {
  {
    const auto up1Only = bmmidi::PitchBendRange::asymmetrical(/* down: */ 0, /* up: */ 1);
    EXPECT_THAT(up1Only.maxSemitonesDown(), Eq(0.0));
    EXPECT_THAT(up1Only.maxSemitonesUp(), Eq(1.0));
  }

  {
    const auto down2_5Only = bmmidi::PitchBendRange::asymmetrical(/* down: */ 2.5, /* up: */ 0);
    EXPECT_THAT(down2_5Only.maxSemitonesDown(), Eq(2.5));
    EXPECT_THAT(down2_5Only.maxSemitonesUp(), Eq(0.0));
  }

  {
    const auto down12Up2 = bmmidi::PitchBendRange::asymmetrical(/* down: */ 12, /* up: */ 2);
    EXPECT_THAT(down12Up2.maxSemitonesDown(), Eq(12.0));
    EXPECT_THAT(down12Up2.maxSemitonesUp(), Eq(2.0));
  }
}

TEST(PitchBendRange, ShouldProvideInRangeTest) {
  {
    // Should work for a symmetrical range.
    const auto range = bmmidi::PitchBendRange::defaultWholeStep();
    EXPECT_THAT(range.canBendBySemitones(-3), IsFalse());
    EXPECT_THAT(range.canBendBySemitones(-2.00000001), IsFalse());
    EXPECT_THAT(range.canBendBySemitones(-2.0), IsTrue());
    EXPECT_THAT(range.canBendBySemitones(-1.5), IsTrue());
    EXPECT_THAT(range.canBendBySemitones(-0.0), IsTrue());
    EXPECT_THAT(range.canBendBySemitones(0.0), IsTrue());
    EXPECT_THAT(range.canBendBySemitones(0.5), IsTrue());
    EXPECT_THAT(range.canBendBySemitones(2.0), IsTrue());
    EXPECT_THAT(range.canBendBySemitones(2.00000001), IsFalse());
    EXPECT_THAT(range.canBendBySemitones(2.5), IsFalse());
  }

  {
    // Should work for an asymmetrical range.
    const auto range = bmmidi::PitchBendRange::asymmetrical(/* down: */ 2.5, /* up: */ 5);
    EXPECT_THAT(range.canBendBySemitones(-3), IsFalse());
    EXPECT_THAT(range.canBendBySemitones(-2.50000001), IsFalse());
    EXPECT_THAT(range.canBendBySemitones(-2.5), IsTrue());
    EXPECT_THAT(range.canBendBySemitones(-1.5), IsTrue());
    EXPECT_THAT(range.canBendBySemitones(-0.0), IsTrue());
    EXPECT_THAT(range.canBendBySemitones(0.0), IsTrue());
    EXPECT_THAT(range.canBendBySemitones(3.5), IsTrue());
    EXPECT_THAT(range.canBendBySemitones(5.0), IsTrue());
    EXPECT_THAT(range.canBendBySemitones(5.00000001), IsFalse());
    EXPECT_THAT(range.canBendBySemitones(5.5), IsFalse());
  }
}

TEST(PitchBend, ShouldSupportAllValues) {
  // Should work for values 0 through 16383.
  {
    // Value 0 should be re-mapped to 0, for symmetry.
    const auto v0 = bmmidi::PitchBend{0};
    EXPECT_THAT(v0.value(), Eq(1));
  }

  {
    const auto v1 = bmmidi::PitchBend{1};
    EXPECT_THAT(v1.value(), Eq(1));
  }

  {
    const auto v3427 = bmmidi::PitchBend{3427};
    EXPECT_THAT(v3427.value(), Eq(3427));
  }

  {
    const auto v8192 = bmmidi::PitchBend{8192};
    EXPECT_THAT(v8192.value(), Eq(8192));
  }

  {
    const auto v15243 = bmmidi::PitchBend{15243};
    EXPECT_THAT(v15243.value(), Eq(15243));
  }

  {
    const auto v16383 = bmmidi::PitchBend{16383};
    EXPECT_THAT(v16383.value(), Eq(16383));
  }

  // Should provide min(), midpoint(), and max() for convenience.
  EXPECT_THAT(bmmidi::PitchBend::min().value(), Eq(1));  // 1, not 0.
  EXPECT_THAT(bmmidi::PitchBend::midpoint().value(), Eq(8192));
  EXPECT_THAT(bmmidi::PitchBend::max().value(), Eq(16383));
}

TEST(PitchBend, ShouldConvertToFromLsbMsb) {
  {
    // LSB:            0b000 0000 (  0x00 = 0)
    // MSB:   0b00 0000 0         (  0x00 = 0)
    // Val: 0b  00 0000 0000 0000 (0x0000 = 0)
    const auto v0 = bmmidi::PitchBend::fromLsbMsb(0x00, 0x00);
    // 0 should still be re-mapped to 1.
    EXPECT_THAT(v0.value(), Eq(1));
  }

  {
    // LSB:            0b000 0001 (  0x01 = 1)
    // MSB:   0b00 0000 0         (  0x00 = 0)
    // Val: 0b  00 0000 0000 0001 (0x0001 = 1)
    const auto v1 = bmmidi::PitchBend::fromLsbMsb(0x01, 0x00);
    EXPECT_THAT(v1.value(), Eq(0x0001));
    EXPECT_THAT(v1.lsb(), Eq(0x01));
    EXPECT_THAT(v1.msb(), Eq(0x00));
  }

  {
    // LSB:            0b110 0011 (  0x63 = 99)
    // MSB:   0b00 1101 0         (  0x1A = 26)
    // Val: 0b  00 1101 0110 0011 (0x0D63 = 3427)
    const auto v3427 = bmmidi::PitchBend::fromLsbMsb(0x63, 0x1A);
    EXPECT_THAT(v3427.value(), Eq(0x0D63));
    EXPECT_THAT(v3427.lsb(), Eq(0x63));
    EXPECT_THAT(v3427.msb(), Eq(0x1A));
  }

  {
    // LSB:            0b000 0000 (  0x00 = 0)
    // MSB:   0b10 0000 0         (  0x40 = 64)
    // Val: 0b  10 0000 0000 0000 (0x2000 = 8192)
    const auto v8192 = bmmidi::PitchBend::fromLsbMsb(0x00, 0x40);
    EXPECT_THAT(v8192.value(), Eq(0x2000));
    EXPECT_THAT(v8192.lsb(), Eq(0x00));
    EXPECT_THAT(v8192.msb(), Eq(0x40));
  }

  {
    // LSB:            0b000 1011 (  0x0B = 11)
    // MSB:   0b11 1011 1         (  0x77 = 119)
    // Val: 0b  11 1011 1000 1011 (0x3B8B = 15243)
    const auto v15243 = bmmidi::PitchBend::fromLsbMsb(0x0B, 0x77);
    EXPECT_THAT(v15243.value(), Eq(0x3B8B));
    EXPECT_THAT(v15243.lsb(), Eq(0x0B));
    EXPECT_THAT(v15243.msb(), Eq(0x77));
  }

  {
    // LSB:            0b111 1111 (  0x7F = 127)
    // MSB:   0b11 1111 1         (  0x7F = 127)
    // Val: 0b  11 1111 1111 1111 (0x3FFF = 16383)
    const auto v16383 = bmmidi::PitchBend::fromLsbMsb(0x7F, 0x7F);
    EXPECT_THAT(v16383.value(), Eq(0x3FFF));
    EXPECT_THAT(v16383.lsb(), Eq(0x7F));
    EXPECT_THAT(v16383.msb(), Eq(0x7F));
  }
}

TEST(PitchBend, ShouldSupportComparisons) {
  // Initialized value of 0 should yield the same value as 1.
  EXPECT_THAT(bmmidi::PitchBend{0} == bmmidi::PitchBend{1}, IsTrue());

  EXPECT_THAT(bmmidi::PitchBend{1} == bmmidi::PitchBend{1}, IsTrue());
  EXPECT_THAT(bmmidi::PitchBend{1} != bmmidi::PitchBend{1}, IsFalse());
  EXPECT_THAT(bmmidi::PitchBend{1} < bmmidi::PitchBend{1}, IsFalse());
  EXPECT_THAT(bmmidi::PitchBend{1} <= bmmidi::PitchBend{1}, IsTrue());
  EXPECT_THAT(bmmidi::PitchBend{1} > bmmidi::PitchBend{1}, IsFalse());
  EXPECT_THAT(bmmidi::PitchBend{1} >= bmmidi::PitchBend{1}, IsTrue());

  EXPECT_THAT(bmmidi::PitchBend{1} == bmmidi::PitchBend{2}, IsFalse());
  EXPECT_THAT(bmmidi::PitchBend{1} != bmmidi::PitchBend{2}, IsTrue());
  EXPECT_THAT(bmmidi::PitchBend{1} < bmmidi::PitchBend{2}, IsTrue());
  EXPECT_THAT(bmmidi::PitchBend{1} <= bmmidi::PitchBend{2}, IsTrue());
  EXPECT_THAT(bmmidi::PitchBend{1} > bmmidi::PitchBend{2}, IsFalse());
  EXPECT_THAT(bmmidi::PitchBend{1} >= bmmidi::PitchBend{2}, IsFalse());

  EXPECT_THAT(bmmidi::PitchBend{16383} == bmmidi::PitchBend{1}, IsFalse());
  EXPECT_THAT(bmmidi::PitchBend{16383} != bmmidi::PitchBend{1}, IsTrue());
  EXPECT_THAT(bmmidi::PitchBend{16383} < bmmidi::PitchBend{1}, IsFalse());
  EXPECT_THAT(bmmidi::PitchBend{16383} <= bmmidi::PitchBend{1}, IsFalse());
  EXPECT_THAT(bmmidi::PitchBend{16383} > bmmidi::PitchBend{1}, IsTrue());
  EXPECT_THAT(bmmidi::PitchBend{16383} >= bmmidi::PitchBend{1}, IsTrue());
}

TEST(PitchBend, ShouldConvertToFromDoubleDataValue) {
  {
    bmmidi::DoubleDataValue v0 = bmmidi::PitchBend{0};
    EXPECT_THAT(v0.value(), Eq(1));

    EXPECT_THAT(bmmidi::PitchBend{v0}.value(), Eq(1));

    bmmidi::PitchBend reassignedTo0{1234};
    reassignedTo0 = bmmidi::DoubleDataValue{0};
    EXPECT_THAT(reassignedTo0.value(), Eq(1));
  }

  {
    bmmidi::DoubleDataValue v8192 = bmmidi::PitchBend::midpoint();
    EXPECT_THAT(v8192.value(), Eq(8192));

    EXPECT_THAT(bmmidi::PitchBend{v8192}.value(), Eq(8192));

    bmmidi::PitchBend reassignedTo8192{1234};
    reassignedTo8192 = bmmidi::DoubleDataValue{8192};
    EXPECT_THAT(reassignedTo8192.value(), Eq(8192));
  }

  {
    bmmidi::DoubleDataValue v16383 = bmmidi::PitchBend::max();
    EXPECT_THAT(v16383.value(), Eq(16383));

    EXPECT_THAT(bmmidi::PitchBend{v16383}.value(), Eq(16383));

    bmmidi::PitchBend reassignedTo16383{1234};
    reassignedTo16383 = bmmidi::DoubleDataValue{16383};
    EXPECT_THAT(reassignedTo16383.value(), Eq(16383));
  }
}

TEST(PitchBend, ShouldConvertToFromSemitonesSymmetrical) {
  const auto defaultRange = bmmidi::PitchBendRange::defaultWholeStep();
  const auto semitoneRange = bmmidi::PitchBendRange::symmetrical(1);

  {
    const auto downNegInf = bmmidi::PitchBend::semitoneBend(kNegInfinity, defaultRange);
    EXPECT_THAT(downNegInf.value(), Eq(1));

    EXPECT_THAT(downNegInf.semitonesWithin(defaultRange), DoubleEq(-2.0));
    EXPECT_THAT(downNegInf.semitonesWithin(semitoneRange), DoubleEq(-1.0));
  }

  {
    const auto downMoreThanMax2 = bmmidi::PitchBend::semitoneBend(-12.345, defaultRange);
    EXPECT_THAT(downMoreThanMax2.value(), Eq(1));

    EXPECT_THAT(downMoreThanMax2.semitonesWithin(defaultRange), DoubleEq(-2.0));
    EXPECT_THAT(downMoreThanMax2.semitonesWithin(semitoneRange), DoubleEq(-1.0));
  }

  {
    const auto downMax2 = bmmidi::PitchBend::semitoneBend(-2.0, defaultRange);
    EXPECT_THAT(downMax2.value(), Eq(1));

    EXPECT_THAT(downMax2.semitonesWithin(defaultRange), DoubleEq(-2.0));
    EXPECT_THAT(downMax2.semitonesWithin(semitoneRange), DoubleEq(-1.0));
  }

  {
    const auto down73c = bmmidi::PitchBend::semitoneBend(-0.73, defaultRange);
    EXPECT_THAT(down73c.value(), Eq(5202));

    EXPECT_THAT(down73c.semitonesWithin(defaultRange), DoubleEq(-0.73006959));
    EXPECT_THAT(down73c.semitonesWithin(semitoneRange), DoubleEq(-0.36503479));
  }

  {
    const auto noBend = bmmidi::PitchBend::semitoneBend(0.0, defaultRange);
    EXPECT_THAT(noBend.value(), Eq(8192));

    EXPECT_THAT(noBend.semitonesWithin(defaultRange), DoubleEq(0.0));
    EXPECT_THAT(noBend.semitonesWithin(semitoneRange), DoubleEq(0.0));
  }

  {
    const auto alsoNoBend = bmmidi::PitchBend::semitoneBend(-0.0, defaultRange);  // Negative zero.
    EXPECT_THAT(alsoNoBend.value(), Eq(8192));

    EXPECT_THAT(alsoNoBend.semitonesWithin(defaultRange), DoubleEq(0.0));
    EXPECT_THAT(alsoNoBend.semitonesWithin(semitoneRange), DoubleEq(0.0));
  }

  {
    const auto up123c = bmmidi::PitchBend::semitoneBend(1.23, defaultRange);
    EXPECT_THAT(up123c.value(), Eq(13229));

    EXPECT_THAT(up123c.semitonesWithin(defaultRange), DoubleEq(1.22988646));
    EXPECT_THAT(up123c.semitonesWithin(semitoneRange), DoubleEq(0.61494323));
  }

  {
    const auto upMax2 = bmmidi::PitchBend::semitoneBend(2.0, defaultRange);
    EXPECT_THAT(upMax2.value(), Eq(16383));

    EXPECT_THAT(upMax2.semitonesWithin(defaultRange), DoubleEq(2.0));
    EXPECT_THAT(upMax2.semitonesWithin(semitoneRange), DoubleEq(1.0));
  }

  {
    const auto upMoreThanMax2 = bmmidi::PitchBend::semitoneBend(12.345, defaultRange);
    EXPECT_THAT(upMoreThanMax2.value(), Eq(16383));

    EXPECT_THAT(upMoreThanMax2.semitonesWithin(defaultRange), DoubleEq(2.0));
    EXPECT_THAT(upMoreThanMax2.semitonesWithin(semitoneRange), DoubleEq(1.0));
  }

  {
    const auto upInf = bmmidi::PitchBend::semitoneBend(kInfinity, defaultRange);
    EXPECT_THAT(upInf.value(), Eq(16383));

    EXPECT_THAT(upInf.semitonesWithin(defaultRange), DoubleEq(2.0));
    EXPECT_THAT(upInf.semitonesWithin(semitoneRange), DoubleEq(1.0));
  }
}

TEST(PitchBend, ShouldConvertToFromSemitonesAsymmetrical) {
  const auto down12Up2 = bmmidi::PitchBendRange::asymmetrical(/* down: */ 12, /* up: */ 2);
  const auto down1Up4 = bmmidi::PitchBendRange::asymmetrical(/* down: */ 1, /* up: */ 4);

  {
    const auto downNegInf = bmmidi::PitchBend::semitoneBend(kNegInfinity, down12Up2);
    EXPECT_THAT(downNegInf.value(), Eq(1));

    EXPECT_THAT(downNegInf.semitonesWithin(down12Up2), DoubleEq(-12.0));
    EXPECT_THAT(downNegInf.semitonesWithin(down1Up4), DoubleEq(-1.0));
  }

  {
    const auto downMoreThanMax12 = bmmidi::PitchBend::semitoneBend(-12.345, down12Up2);
    EXPECT_THAT(downMoreThanMax12.value(), Eq(1));

    EXPECT_THAT(downMoreThanMax12.semitonesWithin(down12Up2), DoubleEq(-12.0));
    EXPECT_THAT(downMoreThanMax12.semitonesWithin(down1Up4), DoubleEq(-1.0));
  }

  {
    const auto downMax12 = bmmidi::PitchBend::semitoneBend(-12.0, down12Up2);
    EXPECT_THAT(downMax12.value(), Eq(1));

    EXPECT_THAT(downMax12.semitonesWithin(down12Up2), DoubleEq(-12.0));
    EXPECT_THAT(downMax12.semitonesWithin(down1Up4), DoubleEq(-1.0));
  }

  {
    const auto down73c = bmmidi::PitchBend::semitoneBend(-0.73, down12Up2);
    EXPECT_THAT(down73c.value(), Eq(7694));

    EXPECT_THAT(down73c.semitonesWithin(down12Up2), DoubleEq(-0.72958125));
    EXPECT_THAT(down73c.semitonesWithin(down1Up4), DoubleEq(-0.06079844));
  }

  {
    const auto noBend = bmmidi::PitchBend::semitoneBend(0.0, down12Up2);
    EXPECT_THAT(noBend.value(), Eq(8192));

    EXPECT_THAT(noBend.semitonesWithin(down12Up2), DoubleEq(0.0));
    EXPECT_THAT(noBend.semitonesWithin(down1Up4), DoubleEq(0.0));
  }

  {
    const auto alsoNoBend = bmmidi::PitchBend::semitoneBend(-0.0, down12Up2);  // Negative zero.
    EXPECT_THAT(alsoNoBend.value(), Eq(8192));

    EXPECT_THAT(alsoNoBend.semitonesWithin(down12Up2), DoubleEq(0.0));
    EXPECT_THAT(alsoNoBend.semitonesWithin(down1Up4), DoubleEq(0.0));
  }

  {
    const auto up123c = bmmidi::PitchBend::semitoneBend(1.23, down12Up2);
    EXPECT_THAT(up123c.value(), Eq(13229));

    EXPECT_THAT(up123c.semitonesWithin(down12Up2), DoubleEq(1.22988646));
    EXPECT_THAT(up123c.semitonesWithin(down1Up4), DoubleEq(2.45977292));
  }

  {
    const auto upMax2 = bmmidi::PitchBend::semitoneBend(2.0, down12Up2);
    EXPECT_THAT(upMax2.value(), Eq(16383));

    EXPECT_THAT(upMax2.semitonesWithin(down12Up2), DoubleEq(2.0));
    EXPECT_THAT(upMax2.semitonesWithin(down1Up4), DoubleEq(4.0));
  }

  {
    const auto upMoreThanMax2 = bmmidi::PitchBend::semitoneBend(12.345, down12Up2);
    EXPECT_THAT(upMoreThanMax2.value(), Eq(16383));

    EXPECT_THAT(upMoreThanMax2.semitonesWithin(down12Up2), DoubleEq(2.0));
    EXPECT_THAT(upMoreThanMax2.semitonesWithin(down1Up4), DoubleEq(4.0));
  }

  {
    const auto upInf = bmmidi::PitchBend::semitoneBend(kInfinity, down12Up2);
    EXPECT_THAT(upInf.value(), Eq(16383));

    EXPECT_THAT(upInf.semitonesWithin(down12Up2), DoubleEq(2.0));
    EXPECT_THAT(upInf.semitonesWithin(down1Up4), DoubleEq(4.0));
  }
}

}  // namespace
