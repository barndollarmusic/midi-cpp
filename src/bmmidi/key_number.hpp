#ifndef BMMIDI_KEY_NUMBER_HPP
#define BMMIDI_KEY_NUMBER_HPP

#include <cassert>
#include <compare>

namespace bmmidi {

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

  /** Returns last valid normal KeyNumber, with value 127. */
  static constexpr KeyNumber last() { return KeyNumber{kMaxNormal}; }

  /** Creates a regular KeyNumber for the given [0, 127] value. */
  static constexpr KeyNumber key(int value) {
    assert(kMinNormal <= value);
    assert(value <= kMaxNormal);
    return KeyNumber{value};
  }

  /** Creates a special KeyNumber representing no key. */
  static constexpr KeyNumber none() { return KeyNumber{kNone}; }

  /** Creates a special KeyNumber representing all keys. */
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

  /**
   * Numerically compares to another KeyNumber, with 'none' treated as less
   * than all normal keys and 'all' treated as greater.
   */
  auto operator<=>(const KeyNumber&) const = default;

private:
  static constexpr int kNone = -1;
  static constexpr int kMinNormal = 0;
  static constexpr int kMaxNormal = 127;
  static constexpr int kAll = 192;

  explicit constexpr KeyNumber(int value) : value_{value} {}

  int value_;
};

}  // namespace bmmidi

#endif  // BMMIDI_KEY_NUMBER_HPP
