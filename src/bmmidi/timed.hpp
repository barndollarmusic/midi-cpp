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

/**
 * Special subclass of Timed<> for wrapped Msg<> or MsgReference<> classes,
 * supporting conversions to more specific timed message and reference types.
 */
template<typename MsgT>
class TimedMsg : public Timed<MsgT> {
public:
  template<typename... Args>
  explicit TimedMsg(double timestamp, Args&&... args)
      : Timed<MsgT>{timestamp, std::forward<Args>(args)...} {}

  /**
   * Returns a copy of this TimedMsg wrapping a more specific subclass SubMsgT;
   * check message type() to ensure that the conversion is valid before calling
   * (in debug mode, assertions will check).
   *
   * This is only supported for wrapped Msg<> subclasses (not MsgReference<>).
   */
  template<typename SubMsgT>
  TimedMsg<SubMsgT> to() const {
    return TimedMsg<SubMsgT>{timestamp(), value().to<SubMsgT>()};
  }

  /**
   * Returns TimedMsg wrapping a read-only view of ViewType; check type()
   * to ensure that the conversion is valid before calling (in debug mode,
   * assertions will check).
   */
  template<typename ViewType>
  TimedMsg<ViewType> asView() {
    return TimedMsg<ViewType>{timestamp(), value().asView<ViewType>()};
  }

  /**
   * Returns TimedMsg wrapping a read-write reference of RefType; check type()
   * to ensure that the conversion is valid before calling (in debug mode,
   * assertions will check).
   *
   * This is only supported if this TimedMsg instance is wrapping a read-write
   * value.
   *
   * Also note that current timestamp will be copied into the new reference, so
   * the timestamp of the original can NOT be changed through this returned
   * value.
   */
  template<typename RefType>
  TimedMsg<RefType> asRef() {
    return TimedMsg<RefType>{timestamp(), value().asRef<RefType>()};
  }
};

}  // namespace bmmidi

#endif  // BMMIDI_TIMED_HPP
