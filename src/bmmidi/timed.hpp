// SPDX-FileCopyrightText: 2022 Barndollar Music, Ltd.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef BMMIDI_TIMED_HPP
#define BMMIDI_TIMED_HPP

#include <cassert>
#include <type_traits>
#include <utility>

namespace bmmidi {

/**
 * Wrapper class template for a value of type T along with a timestamp. Designed
 * to wrap small pass-by-value types.
 *
 * Timestamp is provided as a general double value, and the units and
 * interpretation of values is context dependent. A common use is as an audio
 * sample index.
 */
template<typename T>
class Timed {
public:
  template<typename... Args>
  explicit Timed(double timestamp, Args&&... args)
      : timestamp_{timestamp}, value_{std::forward<Args>(args)...} {}

  /**
   * Returns timestamp. The units and interpretation of values is context
   * dependent. A common use is as an audio sample index.
   */
  double timestamp() const { return timestamp_; }

  /** Updates timestamp. */
  void setTimestamp(double timestamp) { timestamp_ = timestamp; }

  /** Returns reference to wrapped value. */
  T& value() { return value_; }

  /** Returns read-only reference to wrapped value. */
  const T& value() const { return value_; }

private:
  double timestamp_;
  T value_;
};

// Equality operations:
template<typename T>
inline bool operator==(Timed<T> lhs, Timed<T> rhs) {
  return (lhs.timestamp() == rhs.timestamp())
      && (lhs.value() == rhs.value());
}

template<typename T>
inline bool operator!=(Timed<T> lhs, Timed<T> rhs) { return !(lhs == rhs); }

}  // namespace bmmidi

#endif  // BMMIDI_TIMED_HPP
