#ifndef BMMIDI_MSG_HPP
#define BMMIDI_MSG_HPP

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "bmmidi/data_value.hpp"
#include "bmmidi/status.hpp"

namespace bmmidi {

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

  template<typename = std::enable_if_t<N == 1>>
  explicit Msg(Status status)
      : data_{status.value()} {}

  template<typename = std::enable_if_t<N == 2>>
  explicit Msg(Status status, DataValue data1)
      : data_{status.value(), static_cast<std::uint8_t>(data1.value())} {}

  template<typename = std::enable_if_t<N == 3>>
  explicit Msg(Status status, DataValue data1, DataValue data2)
      : data_{status.value(),
              static_cast<std::uint8_t>(data1.value()),
              static_cast<std::uint8_t>(data2.value())} {}

  /** Returns Status byte (first byte of this MIDI message). */
  Status status() const { return Status{data_[0]}; }

  /** Returns [0, 127] value of the 1st data byte. */
  template<typename = std::enable_if_t<N >= 2>>
  DataValue data1() const {
    return DataValue{static_cast<std::int8_t>(data_[1])};
  }

  /** Returns [0, 127] value of the 2nd data byte. */
  template<typename = std::enable_if_t<N >= 3>>
  DataValue data2() const {
    return DataValue{static_cast<std::int8_t>(data_[2])};
  }

private:
  std::uint8_t data_[N];
};

// Ensure compiler keeps these structs packed.
// (It should for char[]; hopefully std::uint8_t[] is the same).
static_assert(sizeof(bmmidi::Msg<1>) == 1, "Msg<1> must be 1 byte");
static_assert(sizeof(bmmidi::Msg<2>) == 2, "Msg<2> must be 2 bytes");
static_assert(sizeof(bmmidi::Msg<3>) == 3, "Msg<3> must be 3 bytes");

}  // namespace bmmidi

#endif  // BMMIDI_MSG_HPP
