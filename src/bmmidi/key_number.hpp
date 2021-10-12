#ifndef BMMIDI_KEY_NUMBER_HPP
#define BMMIDI_KEY_NUMBER_HPP

#include <cassert>

#include "bmmidi/cpp_features.hpp"

namespace bmmidi {

/** The # of discrete MIDI key numbers. */
BMMIDI_INLINE_VAR static constexpr int kNumKeys = 128;

/**
 * Represents a discrete piano keyboard key in [0, 127] range, where middle C
 * (C4 in Scientific Pitch Notation; though other octave numbering standards are
 * also used) has a nominal value of 60.
 *
 * Also allows special "none" and "all" values, which may be valid in certain
 * contexts.
 */
class KeyNumber {
public:
  /** Returns first valid normal KeyNumber, with value 0. */
  static constexpr KeyNumber first() { return KeyNumber{kMinNormal}; }

  /** Returns KeyNumber for middle C, with value 60. */
  static constexpr KeyNumber middleC() { return KeyNumber{60}; }

  /** Returns KeyNumber for tuning A, with value 69. */
  static constexpr KeyNumber tuningA() { return KeyNumber{69}; }

  /** Returns last valid normal KeyNumber, with value 127. */
  static constexpr KeyNumber last() { return KeyNumber{kMaxNormal}; }

  /** Creates a regular KeyNumber for the given [0, 127] value. */
  static constexpr KeyNumber key(int value) {
    assert(kMinNormal <= value);
    assert(value <= kMaxNormal);
    return KeyNumber{value};
  }

  /**
   * Creates a special KeyNumber representing no key.
   *
   * Note that for comparison and increment/decrement operations,
   * KeyNumber::none() represents one-beyond-the-last normal key.
   */
  static constexpr KeyNumber none() { return KeyNumber{kNone}; }

  /**
   * Creates a special KeyNumber representing all keys.
   *
   * Note that it is an invalid operation to increment or decrement the returned
   * KeyNumber::all() value (this fails assertions in debug mode).
   */
  static constexpr KeyNumber all() { return KeyNumber{kAll}; }

  /** Returns true if this is a normal, single [0, 127] MIDI key. */
  constexpr bool isNormal() const { return (kMinNormal <= value_) && (value_ <= kMaxNormal); }

  /** Returns true if this is a special value representing no key. */
  constexpr bool isNone() const { return (value_ == kNone); }

  /** Returns true if this is a special value representing all keys. */
  constexpr bool isAll() const { return (value_ == kAll); }

  /** Returns [0, 127] value of this normal KeyNumber. */
  constexpr int value() const {
    assert(isNormal());
    return value_;
  }

  // Comparison operations:
  friend constexpr bool operator==(KeyNumber lhs, KeyNumber rhs) { return lhs.value_ == rhs.value_; }
  friend constexpr bool operator!=(KeyNumber lhs, KeyNumber rhs) { return lhs.value_ != rhs.value_; }
  friend constexpr bool operator<(KeyNumber lhs, KeyNumber rhs) { return lhs.value_ < rhs.value_; }
  friend constexpr bool operator<=(KeyNumber lhs, KeyNumber rhs) { return lhs.value_ <= rhs.value_; }
  friend constexpr bool operator>(KeyNumber lhs, KeyNumber rhs) { return lhs.value_ > rhs.value_; }
  friend constexpr bool operator>=(KeyNumber lhs, KeyNumber rhs) { return lhs.value_ >= rhs.value_; }

  // Increment and decrement operators (supported for normal keys, with none()
  // representing one-beyond-the-last normal key).

  // Pre-increment:
  KeyNumber& operator++() {
    assert(isNormal());
    ++value_;
    return *this;
  }

  // Post-increment:
  KeyNumber operator++(int) {
    KeyNumber copy = *this;
    ++(*this);
    return copy;
  }

  // Pre-decrement:
  KeyNumber& operator--() {
    --value_;
    assert(isNormal());
    return *this;
  }

  // Post-decrement:
  KeyNumber operator--(int) {
    KeyNumber copy = *this;
    --(*this);
    return copy;
  }

private:
  static constexpr int kMinNormal = 0;
  static constexpr int kMaxNormal = 127;
  static constexpr int kNone = 128;  // One beyond the last normal value.
  static constexpr int kAll = 192;

  explicit constexpr KeyNumber(int value) : value_{value} {}

  int value_;
};

}  // namespace bmmidi

#endif  // BMMIDI_KEY_NUMBER_HPP
