#ifndef BMMIDI_MSG_HPP
#define BMMIDI_MSG_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "bmmidi/channel.hpp"
#include "bmmidi/control.hpp"
#include "bmmidi/data_value.hpp"
#include "bmmidi/key_number.hpp"
#include "bmmidi/preset_number.hpp"
#include "bmmidi/status.hpp"
#include "bmmidi/time.hpp"

namespace bmmidi {

/**
 * Base class for an N-byte MIDI message that stores its own bytes contiguously.
 *
 * Memory layout is packed bytes, so it should be safe to use reinterpret_cast<>
 * on existing memory (but see warnings in MsgReference documentation about
 * interleaved System Realtime messages and running status).
 */
template<std::size_t N>
class Msg {
public:
  /** The # of bytes (N) this message contains. */
  static constexpr std::size_t kNumBytes = N;

  /** Read-write raw byte array type. */
  using ByteArray = std::uint8_t[N];

  /** Read-only raw byte array type. */
  using ConstByteArray = const std::uint8_t[N];

  template<std::size_t NN = N, typename = std::enable_if_t<NN == 1>>
  explicit constexpr Msg(Status status)
      : bytes_{status.value()} {
    assert((type() == MsgType::kSystemExclusive)
        || (N == 1 + status.numDataBytes()));
  }

  template<std::size_t NN = N, typename = std::enable_if_t<NN == 2>>
  explicit constexpr Msg(Status status, DataValue data1)
      : bytes_{status.value(), static_cast<std::uint8_t>(data1.value())} {
    assert((type() == MsgType::kSystemExclusive)
        || (N == 1 + status.numDataBytes()));
  }

  template<std::size_t NN = N, typename = std::enable_if_t<NN == 3>>
  explicit constexpr Msg(Status status, DataValue data1, DataValue data2)
      : bytes_{status.value(),
              static_cast<std::uint8_t>(data1.value()),
              static_cast<std::uint8_t>(data2.value())} {
    assert((type() == MsgType::kSystemExclusive)
        || (N == 1 + status.numDataBytes()));
  }

  /** Returns type of this message, as determined by its Status byte. */
  constexpr MsgType type() const { return status().type(); }

  /** Returns Status byte (first byte of this MIDI message). */
  constexpr Status status() const { return Status{bytes_[0]}; }

  /** Updates Status byte to the given value. */
  void setStatus(Status status) { bytes_[0] = status.value(); }

  /** Returns [0, 127] value of the 1st data byte. */
  template<std::size_t NN = N, typename = std::enable_if_t<NN >= 2>>
  constexpr DataValue data1() const {
    return DataValue{static_cast<std::int8_t>(bytes_[1])};
  }

  /** Updates 1st data byte to the given [0, 127] value. */
  template<std::size_t NN = N, typename = std::enable_if_t<NN >= 2>>
  void setData1(DataValue data1) {
    bytes_[1] = static_cast<std::uint8_t>(data1.value());
  }

  /** Returns [0, 127] value of the 2nd data byte. */
  template<std::size_t NN = N, typename = std::enable_if_t<NN >= 3>>
  constexpr DataValue data2() const {
    return DataValue{static_cast<std::int8_t>(bytes_[2])};
  }

  /** Updates 2nd data byte to the given [0, 127] value. */
  template<std::size_t NN = N, typename = std::enable_if_t<NN >= 3>>
  void setData2(DataValue data2) {
    bytes_[2] = static_cast<std::uint8_t>(data2.value());
  }

  /** Returns read-only reference to raw byte array (including Status byte). */
  ConstByteArray& rawBytes() const { return bytes_; }

  /**
   * Returns read-write pointer to first byte.
   *
   * Careful if writing bytes directly, since it is of course possible to write
   * status and data bytes that do not represent a valid MIDI message (which
   * would cause undefined behavior for this bmmidi library).
   */
  ByteArray& rawBytes() { return bytes_; }

  /**
   * Returns a copy of this Msg as more specific subclass MsgT; check type() to
   * ensure that the conversion is valid before calling (in debug mode,
   * assertions will check).
   */
  template<
      typename MsgT,
      typename = std::enable_if_t<std::is_base_of<Msg<N>, MsgT>::value>>
  MsgT to() const { return MsgT::fromMsg(*this); }

  /**
   * Returns read-write reference of RefType to this Msg; check type() to ensure
   * that the conversion is valid before calling (in debug mode, assertions will
   * check).
   */
  template<
      typename RefType,
      typename = std::enable_if_t<RefType::kAccessType == MsgAccess::kReadWrite>>
  RefType asRef() { return RefType{bytes_, N}; }

  /**
   * Returns read-only reference of ViewType to this Msg; check type() to ensure
   * that the conversion is valid before calling (in debug mode, assertions will
   * check).
   */
  template<
      typename ViewType,
      typename = std::enable_if_t<ViewType::kAccessType == MsgAccess::kReadOnly>>
  ViewType asView() const { return ViewType{bytes_, N}; }

private:
  ByteArray bytes_;
};

template<std::size_t N>
constexpr std::size_t Msg<N>::kNumBytes;  // Definition.

// Ensure compiler keeps these structs packed.
// (It should for char[]; hopefully std::uint8_t[] is the same).
static_assert(sizeof(Msg<1>) == 1, "Msg<1> must be 1 byte");
static_assert(sizeof(Msg<2>) == 2, "Msg<2> must be 2 bytes");
static_assert(sizeof(Msg<3>) == 3, "Msg<3> must be 3 bytes");

// Ensure trivial destructors (so they can be skipped):
static_assert(std::is_trivially_destructible<Msg<1>>::value, "Msg<1> must be trivially destructible");
static_assert(std::is_trivially_destructible<Msg<2>>::value, "Msg<2> must be trivially destructible");
static_assert(std::is_trivially_destructible<Msg<3>>::value, "Msg<3> must be trivially destructible");

// Equality operations:
inline constexpr bool operator==(Msg<1> lhs, Msg<1> rhs) {
  return (lhs.status() == rhs.status());
}

inline constexpr bool operator==(Msg<2> lhs, Msg<2> rhs) {
  return (lhs.status() == rhs.status())
      && (lhs.data1() == rhs.data1());
}

inline constexpr bool operator==(Msg<3> lhs, Msg<3> rhs) {
  return (lhs.status() == rhs.status())
      && (lhs.data1() == rhs.data1())
      && (lhs.data2() == rhs.data2());
}

template<std::size_t N>
inline constexpr bool operator!=(Msg<N> lhs, Msg<N> rhs) { return !(lhs == rhs); }

/** Alias for a timestamped N-byte MIDI message. */
template<std::size_t N>
using TimedMsg = Timed<Msg<N>>;

/**
 * Parent class for an N-byte Channel message that stores its own bytes
 * contiguously.
 *
 * Memory layout is packed bytes, so it should be safe to use reinterpret_cast<>
 * on existing memory (but see warnings in MsgReference documentation about
 * interleaved System Realtime messages and running status).
 */
template<std::size_t N>
class ChanMsg : public Msg<N> {
public:
  // All Channel messages have 1 or 2 data bytes.
  static_assert((N == 2) || (N == 3), "ChanMsg<N>: N must be 2 or 3 bytes");

  static ChanMsg<2> fromMsg(const Msg<2>& msg) {
    return ChanMsg<2>{msg.status(), msg.data1()};
  }

  static ChanMsg<3> fromMsg(const Msg<3>& msg) {
    return ChanMsg<3>{msg.status(), msg.data1(), msg.data2()};
  }

  template<std::size_t NN = N, typename = std::enable_if_t<NN == 2>>
  explicit constexpr ChanMsg(Status status, DataValue data1)
      : Msg<N>{status, data1} {
    assert(status.isChannelSpecific());
  }

  template<std::size_t NN = N, typename = std::enable_if_t<NN == 3>>
  explicit constexpr ChanMsg(Status status, DataValue data1, DataValue data2)
      : Msg<N>{status, data1, data2} {
    assert(status.isChannelSpecific());
  }

  /** Returns MIDI channel this Channel message applies to. */
  constexpr Channel channel() const { return this->status().channel(); }

  /**
   * Updates to the given MIDI channel, which must be a normal [0, 15] value
   * (not a special "none" or "omni" value).
   */
  void setChannel(Channel channel) {
    assert(channel.isNormal());
    setStatus(Status::channelVoice(this->type(), channel));
  }

protected:
  // Hide the more general mutation functions, since specific subclasses provide
  // more specific semantic APIs that will limit correct mutations for the
  // appropriate message type.
  using Msg<N>::setStatus;
  using Msg<N>::setData1;
  using Msg<N>::setData2;
};

static_assert(sizeof(ChanMsg<2>) == 2, "ChanMsg<2> must be 2 bytes");
static_assert(sizeof(ChanMsg<3>) == 3, "ChanMsg<3> must be 3 bytes");

static_assert(std::is_trivially_destructible<ChanMsg<2>>::value,
              "ChanMsg<2> must be trivially destructible");
static_assert(std::is_trivially_destructible<ChanMsg<3>>::value,
              "ChanMsg<3> must be trivially destructible");

/** Alias for a timestamped N-byte Channel message. */
template<std::size_t N>
using TimedChanMsg = Timed<ChanMsg<N>>;

/**
 * A Note Off or Note On message that stores its own bytes contiguously.
 *
 * Memory layout is packed bytes, so it should be safe to use reinterpret_cast<>
 * on existing memory (but see warnings in MsgReference documentation about
 * interleaved System Realtime messages and running status).
 */
class NoteMsg : public ChanMsg<3> {
public:
  static NoteMsg fromMsg(const Msg<3>& msg) {
    assert((msg.type() == MsgType::kNoteOff) || (msg.type() == MsgType::kNoteOn));
    return NoteMsg{msg.status(), KeyNumber::key(msg.data1().value()), msg.data2()};
  }

  /**
   * Returns a Note On message with the given channel, key, and velocity. Note
   * thate if velocity is 0, this will interpreted the same as a Note Off.
   *
   * Input channel must be a normal channel (not a special "none" or "omni"
   * value).
   *
   * Input key must be a normal [0, 127] key number (not a special "none" or
   * "all" value).
   */
  static constexpr NoteMsg on(Channel channel, KeyNumber key, DataValue velocity) {
    return NoteMsg{Status::channelVoice(MsgType::kNoteOn, channel), key, velocity};
  }

  /**
   * Returns a Note Off message with the given chanel, key, and velocity
   * (defaults to 0, since most instruments don't respond to different Note Off
   * velocities).
   *
   * Input channel must be a normal channel (not a special "none" or "omni"
   * value).
   *
   * Input key must be a normal [0, 127] key number (not a special "none" or
   * "all" value).
   */
  static constexpr NoteMsg off(
      Channel channel, KeyNumber key, DataValue velocity = DataValue::min()) {
    return NoteMsg{Status::channelVoice(MsgType::kNoteOff, channel), key, velocity};
  }

  /** Returns true if this is a Note On message (with velocity >= 1). */
  constexpr bool isNoteOn() const { return !isNoteOff(); }

  /**
   * Returns true if this is a Note Off message (or a Note On with velocity 0,
   * which should be treated identically).
   */
  constexpr bool isNoteOff() const {
    return (velocity().value() == 0) || (status().type() == MsgType::kNoteOff);
  }

  /** Sets type of this message to Note On or Note Off. */
  void setType(MsgType type) {
    assert((type == MsgType::kNoteOff) || (type == MsgType::kNoteOn));
    setStatus(Status::channelVoice(type, channel()));
  }

  /** Returns key of note that was pressed or released. */
  constexpr KeyNumber key() const { return KeyNumber::key(data1().value()); }

  /**
   * Updates to the given key number, which must be a normal [0, 127] value
   * (not a special "none" or "all" value).
   */
  void setKey(KeyNumber key) {
    assert(key.isNormal());
    setData1(DataValue{static_cast<std::int8_t>(key.value())});
  }

  /** Returns [0, 127] velocity value. */
  constexpr DataValue velocity() const { return data2(); }

  /** Updates to the given [0, 127] velocity value. */
  void setVelocity(DataValue velocity) { setData2(velocity); }

private:
  explicit constexpr NoteMsg(Status status, KeyNumber key, DataValue velocity)
      : ChanMsg<3>{status,
                   DataValue{static_cast<std::int8_t>(key.value())},
                   velocity} {
    assert(key.isNormal());
  }
};

static_assert(sizeof(NoteMsg) == 3, "NoteMsg must be 3 bytes");
static_assert(std::is_trivially_destructible<NoteMsg>::value,
              "NoteMsg must be trivially destructible");

/** Alias for a timestamped Note Off or Note On message. */
using TimedNoteMsg = Timed<NoteMsg>;

/**
 * A Polyphonic Key Pressure message (a.k.a. per-key aftertouch) that stores its
 * own bytes contiguously.
 *
 * Memory layout is packed bytes, so it should be safe to use reinterpret_cast<>
 * on existing memory (but see warnings in MsgReference documentation about
 * interleaved System Realtime messages and running status).
 */
class KeyPressureMsg : public ChanMsg<3> {
public:
  static KeyPressureMsg fromMsg(const Msg<3>& msg) {
    assert(msg.type() == MsgType::kPolyphonicKeyPressure);
    return KeyPressureMsg{msg.status().channel(), KeyNumber::key(msg.data1().value()), msg.data2()};
  }

  /**
   * Returns a Polyphonic Key Pressure message with the given channel, key, and
   * pressure value.
   *
   * Input channel must be a normal channel (not a special "none" or "omni"
   * value).
   *
   * Input key must be a normal [0, 127] key number (not a special "none" or
   * "all" value).
   */
  explicit constexpr KeyPressureMsg(Channel channel, KeyNumber key, DataValue pressure)
      : ChanMsg<3>{Status::channelVoice(MsgType::kPolyphonicKeyPressure, channel),
                   DataValue{static_cast<std::int8_t>(key.value())},
                   pressure} {
    assert(key.isNormal());
  }

  /** Returns key that this key pressure applies to. */
  constexpr KeyNumber key() const { return KeyNumber::key(data1().value()); }

  /**
   * Updates to the given key number, which must be a normal [0, 127] value
   * (not a special "none" or "all" value).
   */
  void setKey(KeyNumber key) {
    assert(key.isNormal());
    setData1(DataValue{static_cast<std::int8_t>(key.value())});
  }

  /** Returns [0, 127] pressure value. */
  constexpr DataValue pressure() const { return data2(); }

  /** Updates to the given [0, 127] pressure value. */
  void setPressure(DataValue pressure) { setData2(pressure); }
};

static_assert(sizeof(KeyPressureMsg) == 3, "KeyPressureMsg must be 3 bytes");
static_assert(std::is_trivially_destructible<KeyPressureMsg>::value,
              "KeyPressureMsg must be trivially destructible");

/** Alias for a timestamped Polyphonic Key Pressure message. */
using TimedKeyPressureMsg = Timed<KeyPressureMsg>;

/**
 * A Control Change message that stores its own bytes contiguously.
 *
 * Memory layout is packed bytes, so it should be safe to use reinterpret_cast<>
 * on existing memory (but see warnings in MsgReference documentation about
 * interleaved System Realtime messages and running status).
 */
class ControlChangeMsg : public ChanMsg<3> {
public:
  static ControlChangeMsg fromMsg(const Msg<3>& msg) {
    assert(msg.type() == MsgType::kControlChange);
    return ControlChangeMsg{msg.status().channel(), controlFromDataValue(msg.data1()), msg.data2()};
  }

  /**
   * Returns a Control Change message with the given channel, control, and value.
   *
   * Input channel must be a normal channel (not a special "none" or "omni"
   * value).
   */
  explicit constexpr ControlChangeMsg(Channel channel, Control control, DataValue value)
      : ChanMsg<3>{Status::channelVoice(MsgType::kControlChange, channel),
                   controlToDataValue(control),
                   value} {}

  /** Returns Control number being changed. */
  constexpr Control control() const { return controlFromDataValue(data1()); }

  /** Updates to the given Control number. */
  void setControl(Control control) { setData1(controlToDataValue(control)); }

  /** Returns [0, 127] control value. */
  constexpr DataValue value() const { return data2(); }

  /** Updates to the given [0, 127] control value. */
  void setValue(DataValue value) { setData2(value); }
};

static_assert(sizeof(ControlChangeMsg) == 3, "ControlChangeMsg must be 3 bytes");
static_assert(std::is_trivially_destructible<ControlChangeMsg>::value,
              "ControlChangeMsg must be trivially destructible");

/** Alias for a timestamped Control Change message. */
using TimedControlChangeMsg = Timed<ControlChangeMsg>;

/**
 * A Program Change message that stores its own bytes contiguously.
 *
 * Memory layout is packed bytes, so it should be safe to use reinterpret_cast<>
 * on existing memory (but see warnings in MsgReference documentation about
 * interleaved System Realtime messages and running status).
 */
class ProgramChangeMsg : public ChanMsg<2> {
public:
  static ProgramChangeMsg fromMsg(const Msg<2>& msg) {
    assert(msg.type() == MsgType::kProgramChange);
    return ProgramChangeMsg{msg.status().channel(), PresetNumber::index(msg.data1().value())};
  }

  /**
   * Returns a Program Change message with the given channel and program.
   *
   * Input channel must be a normal channel (not a special "none" or "omni"
   * value).
   *
   * Input program must be a normal program (not a special "none" value).
   */
  explicit constexpr ProgramChangeMsg(Channel channel, PresetNumber program)
      : ChanMsg<2>{Status::channelVoice(MsgType::kProgramChange, channel),
                   DataValue{static_cast<std::int8_t>(program.index())}} {
    assert(program.isNormal());
  }

  /** Returns program number. */
  constexpr PresetNumber program() const { return PresetNumber::index(data1().value()); }

  /**
   * Updates to the given program number, which must be in normal [0, 127] index
   * range (not a special "none" value).
   */
  void setProgram(PresetNumber program) {
    assert(program.isNormal());
    setData1(DataValue{static_cast<std::int8_t>(program.index())});
  }
};

static_assert(sizeof(ProgramChangeMsg) == 2, "ProgramChangeMsg must be 2 bytes");
static_assert(std::is_trivially_destructible<ProgramChangeMsg>::value,
              "ProgramChangeMsg must be trivially destructible");

/** Alias for a timestamped Program Change message. */
using TimedProgramChangeMsg = Timed<ProgramChangeMsg>;

/**
 * A Channel Pressure message (non-polyphonic aftertouch that affects all keys)
 * that stores its own bytes contiguously.
 *
 * Memory layout is packed bytes, so it should be safe to use reinterpret_cast<>
 * on existing memory (but see warnings in MsgReference documentation about
 * interleaved System Realtime messages and running status).
 */
class ChanPressureMsg : public ChanMsg<2> {
public:
  static ChanPressureMsg fromMsg(const Msg<2>& msg) {
    assert(msg.type() == MsgType::kChannelPressure);
    return ChanPressureMsg{msg.status().channel(), msg.data1()};
  }

  /**
   * Returns a Channel Pressure message with the given channel and pressure.
   *
   * Input channel must be a normal channel (not a special "none" or "omni"
   * value).
   */
  explicit constexpr ChanPressureMsg(Channel channel, DataValue pressure)
      : ChanMsg<2>{Status::channelVoice(MsgType::kChannelPressure, channel), pressure} {}

  /** Returns [0, 127] pressure value. */
  constexpr DataValue pressure() const { return data1(); }

  /** Updates to the given [0, 127] pressure value. */
  void setPressure(DataValue pressure) { setData1(pressure); }
};

static_assert(sizeof(ChanPressureMsg) == 2, "ChanPressureMsg must be 2 bytes");
static_assert(std::is_trivially_destructible<ChanPressureMsg>::value,
              "ChanPressureMsg must be trivially destructible");

/** Alias for a timestamped Channel Pressure message. */
using TimedChanPressureMsg = Timed<ChanPressureMsg>;

}  // namespace bmmidi

#endif  // BMMIDI_MSG_HPP
