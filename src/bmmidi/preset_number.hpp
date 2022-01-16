// SPDX-FileCopyrightText: 2022 Barndollar Music, Ltd.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef BMMIDI_PRESET_NUMBER_HPP
#define BMMIDI_PRESET_NUMBER_HPP

#include <cassert>

#include "bmmidi/cpp_features.hpp"

namespace bmmidi {

/** The # of discrete MIDI PresetNumber values. */
BMMIDI_INLINE_VAR static constexpr int kNumPresets = 128;

/**
 * Represents a MIDI preset number (program number, bank, tuning program, etc.)
 * with display range for users of [1, 128] (represented by indexes [0, 127]).
 *
 * Also allows a special "none" value, which may be valid in certain contexts.
 */
class PresetNumber {
public:
  /** Returns first valid normal PresetNumber, with index 0. */
  static constexpr PresetNumber first() { return PresetNumber{kMinNormal}; }

  /** Returns last valid normal PresetNumber, with index 127. */
  static constexpr PresetNumber last() { return PresetNumber{kMaxNormal}; }

  /** Creates a regular PresetNumber with the given [0, 127] index value. */
  static constexpr PresetNumber index(int index) {
    assert(kMinNormal <= index);
    assert(index <= kMaxNormal);
    return PresetNumber{index};
  }

  /**
   * Creates a special PresetNumber representing no preset.
   *
   * Note that for comparison and increment/decrement operations,
   * PresentNumber::none() represents one-beyond-the-last normal preset.
   */
  static constexpr PresetNumber none() { return PresetNumber{kNone}; }

  /** Returns true if this is a normal [0, 127] index preset number. */
  constexpr bool isNormal() const { return (kMinNormal <= index_) && (index_ <= kMaxNormal); }

  /** Returns true if this represents no preset. */
  constexpr bool isNone() const { return (index_ == kNone); }

  /** Returns 0-based [0, 127] index of this normal PresetNumber. */
  constexpr int index() const {
    assert(isNormal());
    return index_;
  }

  /** Returns 1-based [1, 128] display number, suitable for display to users. */
  constexpr int displayNumber() const { return index() + 1; }

  // Comparison operations:
  friend constexpr bool operator==(PresetNumber lhs, PresetNumber rhs) { return lhs.index_ == rhs.index_; }
  friend constexpr bool operator!=(PresetNumber lhs, PresetNumber rhs) { return lhs.index_ != rhs.index_; }
  friend constexpr bool operator<(PresetNumber lhs, PresetNumber rhs) { return lhs.index_ < rhs.index_; }
  friend constexpr bool operator<=(PresetNumber lhs, PresetNumber rhs) { return lhs.index_ <= rhs.index_; }
  friend constexpr bool operator>(PresetNumber lhs, PresetNumber rhs) { return lhs.index_ > rhs.index_; }
  friend constexpr bool operator>=(PresetNumber lhs, PresetNumber rhs) { return lhs.index_ >= rhs.index_; }

  // Increment and decrement operators (supported for normal presets, with
  // none() representing one-beyond-the-last normal preset).

  // Pre-increment:
  PresetNumber& operator++() {
    assert(isNormal());
    ++index_;
    return *this;
  }

  // Post-increment:
  PresetNumber operator++(int) {
    PresetNumber copy = *this;
    ++(*this);
    return copy;
  }

  // Pre-decrement:
  PresetNumber& operator--() {
    --index_;
    assert(isNormal());
    return *this;
  }

  // Post-decrement:
  PresetNumber operator--(int) {
    PresetNumber copy = *this;
    --(*this);
    return copy;
  }

private:
  static constexpr int kMinNormal = 0;
  static constexpr int kMaxNormal = 127;
  static constexpr int kNone = 128;  // One beyond the last normal value.

  explicit constexpr PresetNumber(int index) : index_{index} {}

  int index_;
};

}  // namespace bmmidi

#endif  // BMMIDI_PRESET_NUMBER_HPP
