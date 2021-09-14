#ifndef BMMIDI_MSG_HPP
#define BMMIDI_MSG_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "bmmidi/channel.hpp"
#include "bmmidi/data_value.hpp"
#include "bmmidi/key_number.hpp"
#include "bmmidi/status.hpp"
#include "bmmidi/time.hpp"

namespace bmmidi {

/** Used as a template parameter to control read-only vs. read-write access. */
enum class MsgAccess {kReadOnly, kReadWrite};

/**
 * A reference to a MIDI message stored as contiguous bytes (which
 * must outlive this reference object). Can either be read-write (see MsgRef
 * alias) or read-only (see MsgView alias), based on AccessType template
 * parameter.
 *
 * For example, this can refer to a Msg<N> instance, a SysExMsg, or to bytes
 * managed by any other MIDI framework.
 *
 * The refererenced message must be a complete message with status byte and
 * all data bytes following contiguously. A raw MIDI byte stream has 2 features
 * that might not meet those requirements: (1) interleaved System Realtime
 * messages (which can appear in the middle of another message) and (2) running
 * status (where repeated messages of the same type don't repeat the status
 * byte).
 *
 * TODO: Add utility functions for converting from a raw MIDI bytes stream into
 * data that meets the requirements above (and utilities for outputting a raw
 * MIDI byte stream that takes advantage of running status).
 */
template<MsgAccess AccessType>
class MsgReference {
public:
  /** The read-only or read-write access type. */
  static constexpr MsgAccess kAccessType = AccessType;

  /** Type of input byte pointer (const or non-const based on AccessType). */
  using BytePointerType = std::conditional_t<AccessType == MsgAccess::kReadOnly,
                                             const std::uint8_t*, std::uint8_t*>;

  /**
   * Constructs a new MsgReference pointing to contiguous MIDI data bytes,
   * starting from Status byte byte[0] and spanning through last data byte
   * (inclusive) of bytes[numBytes - 1].
   *
   * The referenced MIDI data must outlive this MsgReference instance.
   */
  explicit MsgReference(BytePointerType bytes, int numBytes)
      : bytes_{bytes}, numBytes_{numBytes} {
    assert(bytes != nullptr);
    assert(numBytes >= 1);
    assert((type() == MsgType::kSystemExclusive)
        || (numBytes == 1 + status().numDataBytes()));
  }

  /** Returns type of this message, as determined by its Status byte. */
  MsgType type() const { return status().type(); }

  /** Returns Status byte (first byte of this MIDI message). */
  Status status() const { return Status{bytes_[0]}; }

  /** Updates Status byte to the given value. */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setStatus(Status status) { bytes_[0] = status.value(); }

  // (This empty deleted overload allows using declarations in subclasses).
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadOnly>>
  void setStatus() = delete;

  /**
   * Returns [0, 127] value of the 1st data byte. Error to call (fails
   * debug assertion) if this message does not have any data bytes.
   */
  DataValue data1() const {
    assert(status().numDataBytes() >= 1);
    return DataValue{static_cast<std::int8_t>(bytes_[1])};
  }

  /**
   * Updates 1st data byte to the given value. Error to call (fails
   * debug assertion) if this message does not have any data bytes.
   */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setData1(DataValue data1) {
    assert(status().numDataBytes() >= 1);
    bytes_[1] = static_cast<std::uint8_t>(data1.value());
  }

  // (This empty deleted overload allows using declarations in subclasses).
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadOnly>>
  void setData1() = delete;

  /**
   * Returns [0, 127] value of the 2nd data byte. Error to call (fails
   * debug assertion) if this message does not have a 2nd data byte.
   */
  DataValue data2() const {
    assert(status().numDataBytes() >= 2);
    return DataValue{static_cast<std::int8_t>(bytes_[2])};
  }

  /**
   * Updates 2nd data byte to the given value. Error to call (fails
   * debug assertion) if this message does not have a 2nd data byte.
   */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setData2(DataValue data2) {
    assert(status().numDataBytes() >= 2);
    bytes_[2] = static_cast<std::uint8_t>(data2.value());
  }

  // (This empty deleted overload allows using declarations in subclasses).
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadOnly>>
  void setData2() = delete;

  /**
   * Returns the number of bytes that this message contains, including the
   * initial status byte.
   */
  int numBytes() const { return numBytes_; }

  /** Returns read-only pointer to first byte. */
  const std::uint8_t* rawBytes() const { return bytes_; }

  /**
   * Returns pointer to first byte (read-write only if AccessType allows).
   *
   * Careful if writing bytes directly, since it is of course possible to write
   * status and data bytes that do not represent a valid MIDI message (which
   * would cause undefined behavior for this bmmidi library).
   */
  BytePointerType rawBytes() { return bytes_; }

  /**
   * Returns this reference as a read-write RefType; check type() to ensure that
   * the conversion is valid before calling (in debug mode, assertions will
   * check).
   */
  template<
      typename RefType,
      typename = std::enable_if_t<(AccessType == MsgAccess::kReadWrite)
                                  && (RefType::kAccessType == MsgAccess::kReadWrite)>>
  RefType asRef() { return RefType{bytes_, numBytes_}; }

  /**
   * Returns this reference as a read-only ViewType; check type() to ensure that
   * the conversion is valid before calling (in debug mode, assertions will
   * check).
   */
  template<
      typename ViewType,
      typename = std::enable_if_t<ViewType::kAccessType == MsgAccess::kReadOnly>>
  ViewType asView() const { return ViewType{bytes_, numBytes_}; }

  /** Returns true if this message is byte-for-byte identical to otherReference. */
  template<MsgAccess OtherAccessType>
  bool hasSameValueAs(const MsgReference<OtherAccessType>& otherReference) const {
    return (numBytes() == otherReference.numBytes())
        && (std::memcmp(rawBytes(), otherReference.rawBytes(), numBytes()) == 0);
  }

private:
  BytePointerType bytes_;
  int numBytes_;
};

/** Alias for a read-only MsgReference. */
using MsgView = MsgReference<MsgAccess::kReadOnly>;

/** Alias for a read-write MsgReference. */
using MsgRef = MsgReference<MsgAccess::kReadWrite>;

/** Alias for a timestamped read-only reference to a MIDI message. */
using TimedMsgView = Timed<MsgView>;

/** Alias for a timestamped read-write reference to a MIDI message. */
using TimedMsgRef = Timed<MsgRef>;

/**
 * A reference to a Channel message stored as contiguous bytes (which
 * must outlive this reference object). Can either be read-write (see ChanMsgRef
 * alias) or read-only (see ChanMsgView alias), based on AccessType template
 * parameter.
 */
template<MsgAccess AccessType>
class ChanMsgReference : public MsgReference<AccessType> {
public:
  using BytePointerType = typename MsgReference<AccessType>::BytePointerType;

  explicit ChanMsgReference(BytePointerType bytes, int numBytes)
      : MsgReference<AccessType>{bytes, numBytes} {
    assert(this->status().isChannelSpecific());
  }

  /** Returns MIDI channel this Channel message applies to. */
  Channel channel() const { return this->status().channel(); }

  /**
   * Updates to the given MIDI channel, which must be a normal [0, 15] value
   * (not a special "none" or "omni" value).
   */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setChannel(Channel channel) {
    assert(channel.isNormal());
    setStatus(Status::channelVoice(this->type(), channel));
  }

protected:
  // Hide the more general mutation functions, since specific subclasses provide
  // more specific semantic APIs that will limit correct mutations for the
  // appropriate message type.
  using MsgReference<AccessType>::setStatus;
  using MsgReference<AccessType>::setData1;
  using MsgReference<AccessType>::setData2;
};

/** Alias for a read-only ChanMsgReference. */
using ChanMsgView = ChanMsgReference<MsgAccess::kReadOnly>;

/** Alias for a read-write ChanMsgReference. */
using ChanMsgRef = ChanMsgReference<MsgAccess::kReadWrite>;

/** Alias for a timestamped read-only reference to a Channel message. */
using TimedChanMsgView = Timed<ChanMsgView>;

/** Alias for a timestamped read-write reference to a Channel message. */
using TimedChanMsgRef = Timed<ChanMsgRef>;

/**
 * A reference to a Note Off or Note On message stored as contiguous bytes
 * (which must outlive this reference object). Can either be read-write (see
 * NoteMsgRef alias) or read-only (see NoteMsgView alias), based on AccessType
 * template parameter.
 */
template<MsgAccess AccessType>
class NoteMsgReference : public ChanMsgReference<AccessType> {
public:
  using BytePointerType = typename ChanMsgReference<AccessType>::BytePointerType;

  explicit NoteMsgReference(BytePointerType bytes, int numBytes)
      : ChanMsgReference<AccessType>{bytes, numBytes} {
    assert((this->type() == MsgType::kNoteOff) || (this->type() == MsgType::kNoteOn));
  }

  /** Returns true if this is a Note On message (with velocity >= 1). */
  bool isNoteOn() const { return !isNoteOff(); }

  /**
   * Returns true if this is a Note Off message (or a Note On with velocity 0,
   * which should be treated identically).
   */
  bool isNoteOff() const {
    return (velocity().value() == 0) || (this->status().type() == MsgType::kNoteOff);
  }

  /** Sets type of this message to Note On or Note Off. */
  void setType(MsgType type) {
    assert((type == MsgType::kNoteOff) || (type == MsgType::kNoteOn));
    setStatus(Status::channelVoice(type, this->channel()));
  }

  /** Returns key of note that was pressed or released. */
  KeyNumber key() const { return KeyNumber::key(this->data1().value()); }

  /**
   * Updates to the given key number, which must be a normal [0, 127] value
   * (not a special "none" or "all" value).
   */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setKey(KeyNumber key) {
    assert(key.isNormal());
    this->setData1(DataValue{static_cast<std::int8_t>(key.value())});
  }

  /** Returns [0, 127] velocity value. */
  DataValue velocity() const { return this->data2(); }

  /** Updates to the given [0, 127] velocity value. */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setVelocity(DataValue velocity) { this->setData2(velocity); }
};

/** Alias for a read-only NoteMsgReference. */
using NoteMsgView = NoteMsgReference<MsgAccess::kReadOnly>;

/** Alias for a read-write NoteMsgReference. */
using NoteMsgRef = NoteMsgReference<MsgAccess::kReadWrite>;

/** Alias for a timestamped read-only reference to a Note On/Off message. */
using TimedNoteMsgView = Timed<NoteMsgView>;

/** Alias for a timestamped read-write reference to a Note On/Off message. */
using TimedNoteMsgRef = Timed<NoteMsgRef>;

// TODO: Add more specific MsgView/Ref classes.

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
      : ChanMsg{status,
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

// TODO: Add SysExMsg and other more specific message classes.

}  // namespace bmmidi

#endif  // BMMIDI_MSG_HPP
