// SPDX-FileCopyrightText: 2022 Barndollar Music, Ltd.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef BMMIDI_PITCH_BEND_HPP
#define BMMIDI_PITCH_BEND_HPP

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>

#include "bmmidi/data_value.hpp"

namespace bmmidi {

/**
 * Represents a pitch bend range in semitones, allowing for fractional semitones
 * as well as asymmetrical ranges (e.g. down -12 semitones but up +2 semitones).
 * 
 * The center pitch bend wheel position always represents no bend.
 */
class PitchBendRange {
public:
  /** Returns a symmetrical pitch bend range of +/- semitones. */
  static constexpr PitchBendRange symmetrical(double semitones) {
    return PitchBendRange{semitones, semitones};
  }

  /**
   * Returns a potentially asymmetrical pitch bend range of [-maxSemitonesDown,
   * +maxSemitonesUp]. Note that maxSemitonesDown must be a non-negative value.
   */
  static constexpr PitchBendRange asymmetrical(double maxSemitonesDown, double maxSemitonesUp) {
    return PitchBendRange{maxSemitonesDown, maxSemitonesUp};
  }

  /** Returns commonly used default symmetrical +/- 2 semitone pitch bend range. */
  static constexpr PitchBendRange defaultWholeStep() { return symmetrical(2); }

  /** Returns maximum downward bend in semitones (as a non-negative value). */
  constexpr double maxSemitonesDown() const { return maxSemitonesDown_; }

  /** Returns maximum upward bend in semitones (as a non-negative value). */
  constexpr double maxSemitonesUp() const { return maxSemitonesUp_; }

  /**
   * Returns true if the given +/- semitonesOffset is within this
   * PitchBendRange.
   */
  constexpr bool canBendBySemitones(double semitonesOffset) const {
    return (semitonesOffset < 0.0)
        ? (-semitonesOffset <= maxSemitonesDown_)
        : (semitonesOffset <= maxSemitonesUp_);
  }

private:
  explicit constexpr PitchBendRange(double maxSemitonesDown, double maxSemitonesUp)
      : maxSemitonesDown_{maxSemitonesDown},
        maxSemitonesUp_{maxSemitonesUp} {
    assert(maxSemitonesDown >= 0.0);
    assert(maxSemitonesUp >= 0.0);
  }

  double maxSemitonesDown_;
  double maxSemitonesUp_;
};

/** Represents a 14-bit MIDI pitch bend value. */
class PitchBend : public DoubleDataValue {
public:
  /** Returns minimum bend value (1, for symmetry). */
  static constexpr PitchBend min() { return PitchBend{kMin}; }

  /** Returns midpoint bend value (8192). */
  static constexpr PitchBend midpoint() { return PitchBend{kMidpoint}; }

  /** Returns maximum bend value (16383). */
  static constexpr PitchBend max() { return PitchBend{kMax}; }

  /**
   * Creates a PitchBend with value closest to the given semitones offset
   * within the provided pitch bend range. If desired semitones bend is out of
   * the given range, this will cap to the minimum or maximum bend value.
   *
   * Will yield a value in [1, 16383] range (for symmetry).
   */
  static PitchBend semitoneBend(double semitones, PitchBendRange range) {
    const double offsetFraction = (semitones < 0.0)
        ? std::max(-1.0, semitones / range.maxSemitonesDown())
        : std::min(semitones / range.maxSemitonesUp(), 1.0);

    const std::int16_t value =
        static_cast<std::int16_t>(std::round(kMidpoint + offsetFraction * kMaxOffset));
    assert(kMin <= value);
    assert(value <= kMax);

    return PitchBend{value};
  }

  /**
   * Creates a PitchBend from the input lsb (Least Significant Byte,
   * i.e. lower 7 bits) and msb (Most Significant Byte, i.e. upper 7 bits).
   */
  static constexpr PitchBend fromLsbMsb(std::uint8_t lsb, std::uint8_t msb) {
    return PitchBend{DoubleDataValue::fromLsbMsb(lsb, msb).value()};
  }

  /**
   * Creates a PitchBend from the input [0, 16383] value.
   *
   * Maps an input 0 value to 1 (for symmetry).
   */
  explicit constexpr PitchBend(std::int16_t value)
      : DoubleDataValue{(value == 0) ? std::int16_t{1} : value} {}

  constexpr PitchBend(DoubleDataValue rhs)
      : PitchBend{rhs.value()} {}
  PitchBend& operator=(DoubleDataValue rhs) {
    *this = PitchBend{rhs};
    return *this;
  }

  /**
   * Returns intended bend size in semitones (possibly non-integer) that this
   * PitchBend represents assuming the given PitchBendRange.
   */
  double semitonesWithin(PitchBendRange range) const {
    const double offsetFraction = (value() - kMidpoint) / static_cast<double>(kMaxOffset);

    // Not: Take max in downward case to allow for a 0 value (which should be
    // treated the same as 1).
    return (offsetFraction < 0.0)
        ? std::max(-range.maxSemitonesDown(), offsetFraction * range.maxSemitonesDown())
        : offsetFraction * range.maxSemitonesUp();
  }

private:
  static constexpr std::int16_t kMin = 1;
  static constexpr std::int16_t kMidpoint = 8192;
  static constexpr std::int16_t kMax = 16383;

  // Symmetrical +/- 8191 from midpoint.
  static constexpr std::int16_t kMaxOffset = kMax - kMidpoint;

  // IMPORTANT: No data members, so that PitchBend can be safely sliced
  // by value as a DoubleDataValue.
};

static_assert(sizeof(PitchBend) == sizeof(DoubleDataValue),
              "PitchBend must have same size as DoubleDataValue");

}  // namespace bmmidi

#endif  // BMMIDI_PITCH_BEND_HPP
