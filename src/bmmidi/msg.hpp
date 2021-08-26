#ifndef BMMIDI_MSG_HPP
#define BMMIDI_MSG_HPP

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "bmmidi/data_value.hpp"
#include "bmmidi/status.hpp"
#include "bmmidi/time.hpp"

namespace bmmidi {

/**
 * A read-only view of a MIDI message stored as contiguous bytes (which
 * must outlive this reference object).
 *
 * For example, this can refer to a Msg<N> instance, a SysExMsg, or to bytes
 * managed by any other MIDI framework.
 *
 * Make sure MIDI data this is referring to does NOT contain any interleaved
 * System Realtime message bytes, which can occur at any point in a raw MIDI
 * byte stream.
 */
class MsgView {
public:
  /**
   * Constructs a new MsgView pointing to contiguous MIDI data bytes, starting
   * from Status byte byte[0] and spanning through last data byte (inclusive)
   * of bytes[numBytes - 1].
   *
   * The referenced MIDI data must outlive this MsgView instance.
   */
  explicit MsgView(const std::uint8_t* bytes, int numBytes)
      : bytes_{bytes}, numBytes_{numBytes} {
    assert(bytes != nullptr);
    assert(numBytes >= 1);
    assert((type() == MsgType::kSystemExclusive)
        || (numBytes == 1 + status().numDataBytes()));
  }

  /** Returns type of this message, as determined by its Status byte. */
  MsgType type() const { return status().type(); }

  /** Returns Status byte. */
  Status status() const { return Status{bytes_[0]}; }

  /**
   * Returns [0, 127] value of the 1st data byte. Error to call (fails
   * debug assertion) if this message does not have any data bytes.
   */
  DataValue data1() const {
    assert(status().numDataBytes() >= 1);
    return DataValue{static_cast<std::int8_t>(bytes_[1])};
  }

  /**
   * Returns [0, 127] value of the 2nd data byte. Error to call (fails
   * debug assertion) if this message does not have a 2nd data byte.
   */
  DataValue data2() const {
    assert(status().numDataBytes() >= 2);
    return DataValue{static_cast<std::int8_t>(bytes_[2])};
  }

  /**
   * Returns the number of bytes that this message contains, including the
   * initial status byte.
   */
  int numBytes() const { return numBytes_; }

  /** Returns read-only pointer to first byte. */
  const std::uint8_t* rawBytes() const { return bytes_; }

private:
  const std::uint8_t* bytes_;
  int numBytes_;
};

/** Alias for a timestamped read-only view of a MIDI message. */
using TimedMsgView = Timed<MsgView>;

/**
 * A read/write reference to a MIDI message stored as contiguous bytes (which
 * must outlive this reference object).
 *
 * For example, this can refer to a Msg<N> instance, a SysExMsg, or to bytes
 * managed by any other MIDI framework.
 *
 * Make sure MIDI data this is referring to does NOT contain any interleaved
 * System Realtime message bytes, which can occur at any point in a raw MIDI
 * byte stream.
 */
class MsgRef : public MsgView {
public:
  /**
   * Constructs a new MsgRef pointing to contiguous MIDI data bytes, starting
   * from Status byte byte[0] and spanning through last data byte (inclusive)
   * of bytes[numBytes - 1].
   *
   * The referenced MIDI data must outlive this MsgRef instance.
   */
  explicit MsgRef(std::uint8_t* bytes, int numBytes)
      : MsgView{bytes, numBytes} {}

  /** Updates Status byte to the given value. */
  void setStatus(Status status) {
    rawBytes()[0] = status.value();
  }

  /**
   * Updates 1st data byte to the given value. Error to call (fails
   * debug assertion) if this message does not have any data bytes.
   */
  void setData1(DataValue data1) {
    assert(status().numDataBytes() >= 1);
    rawBytes()[1] = static_cast<std::uint8_t>(data1.value());
  }

  /**
   * Updates 2nd data byte to the given value. Error to call (fails
   * debug assertion) if this message does not have a 2nd data byte.
   */
  void setData2(DataValue data2) {
    assert(status().numDataBytes() >= 2);
    rawBytes()[2] = static_cast<std::uint8_t>(data2.value());
  }

  /**
   * Returns read/write pointer to first byte; careful if mutating raw bytes
   * directly, since it is of course possible to mutate into something that
   * does not represent a valid MIDI message (which can cause undefined behavior
   * for this bmmidi library).
   */
  std::uint8_t* rawBytes() {
    // NOTE: This is safe only because we know MsgRef was constructed with a
    // non-const std::uint8_t*. Not storing the non-const pointer separately,
    // since that would be redundant.
    return const_cast<std::uint8_t*>(MsgView::rawBytes());
  }
};

/** Alias for a timestamped read/write reference to a MIDI message. */
using TimedMsgRef = Timed<MsgRef>;

// TODO: Add more specific MsgView/Ref classes.

/**
 * Base class for an N-byte MIDI message that stores its own bytes contiguously.
 *
 * Memory layout is packed bytes, so it should be safe to use reinterpret_cast<>
 * on existing memory (but make sure MIDI data does NOT contain any
 * interleaved System Realtime message bytes, which can occur at any point in a
 * raw MIDI byte stream).
 */
template<std::size_t N>
class Msg {
public:
  /** The # of bytes (N) this message contains. */
  static constexpr std::size_t kNumBytes = N;

  /** Read-only raw byte array type. */
  using ConstByteArray = const std::uint8_t[N];

  template<typename = std::enable_if_t<N == 1>>
  explicit Msg(Status status)
      : bytes_{status.value()} {}

  template<typename = std::enable_if_t<N == 2>>
  explicit Msg(Status status, DataValue data1)
      : bytes_{status.value(), static_cast<std::uint8_t>(data1.value())} {}

  template<typename = std::enable_if_t<N == 3>>
  explicit Msg(Status status, DataValue data1, DataValue data2)
      : bytes_{status.value(),
              static_cast<std::uint8_t>(data1.value()),
              static_cast<std::uint8_t>(data2.value())} {}

  /** Returns type of this message. */
  MsgType type() const { return status().type(); }

  /** Returns Status byte (first byte of this MIDI message). */
  Status status() const { return Status{bytes_[0]}; }

  /** Returns [0, 127] value of the 1st data byte. */
  template<typename = std::enable_if_t<N >= 2>>
  DataValue data1() const {
    return DataValue{static_cast<std::int8_t>(bytes_[1])};
  }

  /** Returns [0, 127] value of the 2nd data byte. */
  template<typename = std::enable_if_t<N >= 3>>
  DataValue data2() const {
    return DataValue{static_cast<std::int8_t>(bytes_[2])};
  }

  /** Returns read-only reference to raw byte array (including Status byte). */
  ConstByteArray& rawBytes() const { return bytes_; }

  /** Returns read-only view of this message. */
  MsgView view() const { return MsgView{bytes_, N}; }

  /** Returns read-write reference to this message. */
  MsgRef ref() { return MsgRef{bytes_, N}; }

private:
  std::uint8_t bytes_[N];
};

// Ensure compiler keeps these structs packed.
// (It should for char[]; hopefully std::uint8_t[] is the same).
static_assert(sizeof(bmmidi::Msg<1>) == 1, "Msg<1> must be 1 byte");
static_assert(sizeof(bmmidi::Msg<2>) == 2, "Msg<2> must be 2 bytes");
static_assert(sizeof(bmmidi::Msg<3>) == 3, "Msg<3> must be 3 bytes");

// Equality operations:
inline bool operator==(Msg<1> lhs, Msg<1> rhs) {
  return (lhs.status() == rhs.status());
}

inline bool operator==(Msg<2> lhs, Msg<2> rhs) {
  return (lhs.status() == rhs.status())
      && (lhs.data1() == rhs.data1());
}

inline bool operator==(Msg<3> lhs, Msg<3> rhs) {
  return (lhs.status() == rhs.status())
      && (lhs.data1() == rhs.data1())
      && (lhs.data2() == rhs.data2());
}

template<std::size_t N>
inline bool operator!=(Msg<N> lhs, Msg<N> rhs) { return !(lhs == rhs); }

/** Alias for a timestamped N-byte MIDI message. */
template<std::size_t N>
using TimedMsg = Timed<Msg<N>>;

// TODO: Add SysExMsg and other more specific message classes.

}  // namespace bmmidi

#endif  // BMMIDI_MSG_HPP
