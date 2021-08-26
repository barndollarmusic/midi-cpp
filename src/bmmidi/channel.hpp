#ifndef BMMIDI_CHANNEL_HPP
#define BMMIDI_CHANNEL_HPP

#include <cassert>

namespace bmmidi {

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

  /** Creates a special Channel representing no channel. */
  static constexpr Channel none() { return Channel{kNone}; }

  /** Creates a special Channel representing all channels (omni). */
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
  friend bool operator==(Channel lhs, Channel rhs) { return lhs.index_ == rhs.index_; }
  friend bool operator!=(Channel lhs, Channel rhs) { return lhs.index_ != rhs.index_; }
  friend bool operator<(Channel lhs, Channel rhs) { return lhs.index_ < rhs.index_; }
  friend bool operator<=(Channel lhs, Channel rhs) { return lhs.index_ <= rhs.index_; }
  friend bool operator>(Channel lhs, Channel rhs) { return lhs.index_ > rhs.index_; }
  friend bool operator>=(Channel lhs, Channel rhs) { return lhs.index_ >= rhs.index_; }

private:
  static constexpr int kNone = -1;
  static constexpr int kMinNormal = 0;
  static constexpr int kMaxNormal = 15;
  static constexpr int kOmni = 64;

  explicit constexpr Channel(int index) : index_{index} {}

  int index_;
};

}  // namespace bmmidi

#endif  // BMMIDI_CHANNEL_HPP
