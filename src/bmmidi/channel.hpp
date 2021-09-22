#ifndef BMMIDI_CHANNEL_HPP
#define BMMIDI_CHANNEL_HPP

#include <cassert>

#include "bmmidi/cpp_features.hpp"

namespace bmmidi {

/** The # of MIDI channels. */
BMMIDI_INLINE_VAR static constexpr int kNumChannels = 16;

/**
 * Represents a MIDI channel, with normal 0-based index in range [0, 15] or
 * one of the special values indicating no channel or omni (all channels).
 */
class Channel {
public:
  /** Returns first valid normal Channel, with index 0. */
  static constexpr Channel first() { return Channel{kMinNormal}; }

  /** Returns last valid normal Channel, with index 15. */
  static constexpr Channel last() { return Channel{kMaxNormal}; }

  /** Creates a regular Channel with the given [0, 15] channelIndex value. */
  static constexpr Channel index(int channelIndex) {
    assert(kMinNormal <= channelIndex);
    assert(channelIndex <= kMaxNormal);
    return Channel{channelIndex};
  }

  /**
   * Creates a special Channel representing no channel.
   *
   * Note that for comparison and increment/decrement operations,
   * Channel::none() represents one-beyond-the-last normal channel.
   */
  static constexpr Channel none() { return Channel{kNone}; }

  /**
   * Creates a special Channel representing all channels (omni).
   *
   * Note that it is an invalid operation to increment or decrement the returned
   * Channel::ommni() value (this fails assertions in debug mode).
   */
  static constexpr Channel omni() { return Channel{kOmni}; }

  /** Returns true if this is a normal, single [0, 15] MIDI channel. */
  constexpr bool isNormal() const { return (kMinNormal <= index_) && (index_ <= kMaxNormal); }

  /** Returns true if this represents no channel. */
  constexpr bool isNone() const { return (index_ == kNone); }

  /** Returns true if this represents all channels (omni). */
  constexpr bool isOmni() const { return (index_ == kOmni); }

  /** Returns 0-based [0, 15] index of this normal Channel. */
  constexpr int index() const {
    assert(isNormal());
    return index_;
  }

  /** Returns 1-based [1, 16] display number, suitable for display to users. */
  constexpr int displayNumber() const { return index() + 1; }

  // Comparison operations:
  friend constexpr bool operator==(Channel lhs, Channel rhs) { return lhs.index_ == rhs.index_; }
  friend constexpr bool operator!=(Channel lhs, Channel rhs) { return lhs.index_ != rhs.index_; }
  friend constexpr bool operator<(Channel lhs, Channel rhs) { return lhs.index_ < rhs.index_; }
  friend constexpr bool operator<=(Channel lhs, Channel rhs) { return lhs.index_ <= rhs.index_; }
  friend constexpr bool operator>(Channel lhs, Channel rhs) { return lhs.index_ > rhs.index_; }
  friend constexpr bool operator>=(Channel lhs, Channel rhs) { return lhs.index_ >= rhs.index_; }

  // Increment and decrement operators (supported for normal channels, with
  // none() representing one-beyond-the-last normal channel).

  // Pre-increment:
  Channel& operator++() {
    assert(isNormal());
    ++index_;
    return *this;
  }

  // Post-increment:
  Channel operator++(int) {
    Channel copy = *this;
    ++(*this);
    return copy;
  }

  // Pre-decrement:
  Channel& operator--() {
    --index_;
    assert(isNormal());
    return *this;
  }

  // Post-decrement:
  Channel operator--(int) {
    Channel copy = *this;
    --(*this);
    return copy;
  }

private:
  static constexpr int kMinNormal = 0;
  static constexpr int kMaxNormal = 15;
  static constexpr int kNone = 16;  // One beyond the last.
  static constexpr int kOmni = 64;

  explicit constexpr Channel(int index) : index_{index} {}

  int index_;
};

}  // namespace bmmidi

#endif  // BMMIDI_CHANNEL_HPP
