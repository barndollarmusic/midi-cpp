#ifndef BMMIDI_MSG_REFERENCE_HPP
#define BMMIDI_MSG_REFERENCE_HPP

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

/**
 * A reference to a Polyphonic Key Pressure message (a.k.a. per-key aftertouch)
 * stored as contiguous bytes (which must outlive this reference object). Can
 * either be read-write (see KeyPressureMsgRef alias) or read-only (see
 * KeyPressureMsgView alias), based on AccessType template parameter.
 */
template<MsgAccess AccessType>
class KeyPressureMsgReference : public ChanMsgReference<AccessType> {
public:
  using BytePointerType = typename ChanMsgReference<AccessType>::BytePointerType;

  explicit KeyPressureMsgReference(BytePointerType bytes, int numBytes)
      : ChanMsgReference<AccessType>{bytes, numBytes} {
    assert(this->type() == MsgType::kPolyphonicKeyPressure);
  }

  /** Returns key that this key pressure applies to. */
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

  /** Returns [0, 127] pressure value. */
  DataValue pressure() const { return this->data2(); }

  /** Updates to the given [0, 127] pressure value. */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setPressure(DataValue pressure) { this->setData2(pressure); }
};

/** Alias for a read-only KeyPressureMsgReference. */
using KeyPressureMsgView = KeyPressureMsgReference<MsgAccess::kReadOnly>;

/** Alias for a read-write KeyPressureMsgReference. */
using KeyPressureMsgRef = KeyPressureMsgReference<MsgAccess::kReadWrite>;

/** Alias for a timestamped read-only reference to a key pressure message. */
using TimedKeyPressureMsgView = Timed<KeyPressureMsgView>;

/** Alias for a timestamped read-write reference to a key pressure message. */
using TimedKeyPressureMsgRef = Timed<KeyPressureMsgRef>;

/**
 * A reference to a Control Change message stored as contiguous bytes (which
 * must outlive this reference object). Can either be read-write (see
 * ControlChangeMsgRef alias) or read-only (see ControlChangeMsgView alias),
 * based on AccessType template parameter.
 */
template<MsgAccess AccessType>
class ControlChangeMsgReference : public ChanMsgReference<AccessType> {
public:
  using BytePointerType = typename ChanMsgReference<AccessType>::BytePointerType;

  explicit ControlChangeMsgReference(BytePointerType bytes, int numBytes)
      : ChanMsgReference<AccessType>{bytes, numBytes} {
    assert(this->type() == MsgType::kControlChange);
  }

  /** Returns Control number being changed. */
  Control control() const { return controlFromDataValue(this->data1()); }

  /** Updates to the given Control number. */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setControl(Control control) {
    this->setData1(controlToDataValue(control));
  }

  /** Returns [0, 127] control value. */
  DataValue value() const { return this->data2(); }

  /** Updates to the given [0, 127] control value. */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setValue(DataValue value) { this->setData2(value); }
};

/** Alias for a read-only ControlChangeMsgReference. */
using ControlChangeMsgView = ControlChangeMsgReference<MsgAccess::kReadOnly>;

/** Alias for a read-write ControlChangeMsgReference. */
using ControlChangeMsgRef = ControlChangeMsgReference<MsgAccess::kReadWrite>;

/** Alias for a timestamped read-only reference to a control change message. */
using TimedControlChangeMsgView = Timed<ControlChangeMsgView>;

/** Alias for a timestamped read-write reference to a control change message. */
using TimedControlChangeMsgRef = Timed<ControlChangeMsgRef>;

/**
 * A reference to a Program Change message stored as contiguous bytes (which
 * must outlive this reference object). Can either be read-write (see
 * ProgramChangeMsgRef alias) or read-only (see ProgramChangeMsgView alias),
 * based on AccessType template parameter.
 */
template<MsgAccess AccessType>
class ProgramChangeMsgReference : public ChanMsgReference<AccessType> {
public:
  using BytePointerType = typename ChanMsgReference<AccessType>::BytePointerType;

  explicit ProgramChangeMsgReference(BytePointerType bytes, int numBytes)
      : ChanMsgReference<AccessType>{bytes, numBytes} {
    assert(this->type() == MsgType::kProgramChange);
  }

  /** Returns program number. */
  PresetNumber program() const { return PresetNumber::index(this->data1().value()); }

  /**
   * Updates to the given program number, which must be in normal [0, 127] index
   * range (not a special "none" value).
   */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setProgram(PresetNumber program) {
    assert(program.isNormal());
    this->setData1(DataValue{static_cast<std::int8_t>(program.index())});
  }
};

/** Alias for a read-only ProgramChangeMsgReference. */
using ProgramChangeMsgView = ProgramChangeMsgReference<MsgAccess::kReadOnly>;

/** Alias for a read-write ProgramChangeMsgReference. */
using ProgramChangeMsgRef = ProgramChangeMsgReference<MsgAccess::kReadWrite>;

/** Alias for a timestamped read-only reference to a program change message. */
using TimedProgramChangeMsgView = Timed<ProgramChangeMsgView>;

/** Alias for a timestamped read-write reference to a program change message. */
using TimedProgramChangeMsgRef = Timed<ProgramChangeMsgRef>;

}  // namespace bmmidi

#endif  // BMMIDI_MSG_REFERENCE_HPP
