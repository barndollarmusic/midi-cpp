#ifndef BMMIDI_MSG_REFERENCE_HPP
#define BMMIDI_MSG_REFERENCE_HPP

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

#include "bmmidi/bitops.hpp"
#include "bmmidi/channel.hpp"
#include "bmmidi/control.hpp"
#include "bmmidi/data_value.hpp"
#include "bmmidi/key_number.hpp"
#include "bmmidi/pitch_bend.hpp"
#include "bmmidi/preset_number.hpp"
#include "bmmidi/status.hpp"
#include "bmmidi/sysex.hpp"
#include "bmmidi/time.hpp"
#include "bmmidi/timecode.hpp"

namespace bmmidi {

/** Used as a template parameter to control read-only vs. read-write access. */
enum class MsgAccess {kReadOnly, kReadWrite};

/**
 * A reference to a MIDI message stored as contiguous bytes (which
 * must outlive this reference object). Can either be read-write (see MsgRef
 * alias) or read-only (see MsgView alias), based on AccessType template
 * parameter.
 *
 * For example, this can refer to a Msg<N> instance, a MfrSysEx or
 * UniversalSysEx message, or to bytes managed by any other MIDI framework.
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

/**
 * A reference to a Channel Pressure message (non-polyphonic aftertouch which
 * applies to all keys) stored as contiguous bytes (which must outlive this
 * reference object). Can either be read-write (see ChanPressureMsgRef alias) or
 * read-only (see ChanPressureMsgView alias), based on AccessType template
 * parameter.
 */
template<MsgAccess AccessType>
class ChanPressureMsgReference : public ChanMsgReference<AccessType> {
public:
  using BytePointerType = typename ChanMsgReference<AccessType>::BytePointerType;

  explicit ChanPressureMsgReference(BytePointerType bytes, int numBytes)
      : ChanMsgReference<AccessType>{bytes, numBytes} {
    assert(this->type() == MsgType::kChannelPressure);
  }

  /** Returns [0, 127] pressure value. */
  DataValue pressure() const { return this->data1(); }

  /** Updates to the given [0, 127] pressure value. */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setPressure(DataValue pressure) { this->setData1(pressure); }
};

/** Alias for a read-only ChanPressureMsgReference. */
using ChanPressureMsgView = ChanPressureMsgReference<MsgAccess::kReadOnly>;

/** Alias for a read-write ChanPressureMsgReference. */
using ChanPressureMsgRef = ChanPressureMsgReference<MsgAccess::kReadWrite>;

/** Alias for a timestamped read-only reference to a channel pressure message. */
using TimedChanPressureMsgView = Timed<ChanPressureMsgView>;

/** Alias for a timestamped read-write reference to a channel pressure message. */
using TimedChanPressureMsgRef = Timed<ChanPressureMsgRef>;

/**
 * A reference to a Pitch Bend Change message stored as contiguous bytes (which
 * must outlive this reference object). Can either be read-write (see
 * PitchBendMsgRef alias) or read-only (see PitchBendMsgView alias), based on
 * AccessType template parameter.
 */
template<MsgAccess AccessType>
class PitchBendMsgReference : public ChanMsgReference<AccessType> {
public:
  using BytePointerType = typename ChanMsgReference<AccessType>::BytePointerType;

  explicit PitchBendMsgReference(BytePointerType bytes, int numBytes)
      : ChanMsgReference<AccessType>{bytes, numBytes} {
    assert(this->type() == MsgType::kPitchBend);
  }

  /** Returns 14-bit [1, 16383] MIDI pitch bend value. */
  PitchBend bend() const {
    return PitchBend::fromLsbMsb(
        static_cast<std::uint8_t>(this->data1().value()),
        static_cast<std::uint8_t>(this->data2().value()));
  }

  /** Updates to the given 14-bit [1, 16383] MIDI pitch bend value. */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setBend(PitchBend bend) {
    this->setData1(DataValue{static_cast<std::int8_t>(bend.lsb())});
    this->setData2(DataValue{static_cast<std::int8_t>(bend.msb())});
  }
};

/** Alias for a read-only PitchBendMsgReference. */
using PitchBendMsgView = PitchBendMsgReference<MsgAccess::kReadOnly>;

/** Alias for a read-write PitchBendMsgReference. */
using PitchBendMsgRef = PitchBendMsgReference<MsgAccess::kReadWrite>;

/** Alias for a timestamped read-only reference to a pitch bend message. */
using TimedPitchBendMsgView = Timed<PitchBendMsgView>;

/** Alias for a timestamped read-write reference to a pitch bend message. */
using TimedPitchBendMsgRef = Timed<PitchBendMsgRef>;

/**
 * A reference to a System Exclusive (SysEx) message stored as contiguous bytes
 * (which must outlive this reference object), including a terminating End of
 * Exclusive (EOX) status byte. Can either be read-write (see SysExMsgRef
 * alias) or read-only (see SysExMsgView alias), based on AccessType template
 * parameter.
 */
template<MsgAccess AccessType>
class SysExMsgReference : public MsgReference<AccessType> {
public:
  using BytePointerType = typename MsgReference<AccessType>::BytePointerType;

  explicit SysExMsgReference(BytePointerType bytes, int numBytes)
      : MsgReference<AccessType>{bytes, numBytes} {
    assert(this->type() == MsgType::kSystemExclusive);
    assert(bytes[numBytes - 1] == static_cast<std::uint8_t>(MsgType::kEndOfSystemExclusive));
  }

  /**
   * Returns SysEx ID, the first data byte of this message (after the status
   * byte), which determines whether this is a manufacturer-specific or
   * universal message.
   */
  std::uint8_t sysExId() const { return this->rawBytes()[1]; }

  // NOTE: NOT providing mutation for sysExId(), since some mutations would
  // change the # of bytes in this message.

  /**
   * Returns true if this is a universal (non-realtime or realtime) SysEx
   * message; otherwise, this is a manufacturer-specific (or private use
   * non-commercial) SysEx message.
   */
  bool isUniversal() const {
    return (sysExId() == static_cast<std::uint8_t>(UniversalCategory::kNonRealTime))
        || (sysExId() == static_cast<std::uint8_t>(UniversalCategory::kRealTime));
  }

protected:
  // Hide the more general mutation and access functions, which don't apply to
  // SysEx messages.
  using MsgReference<AccessType>::setStatus;

  using MsgReference<AccessType>::data1;
  using MsgReference<AccessType>::setData1;

  using MsgReference<AccessType>::data2;
  using MsgReference<AccessType>::setData2;
};

/** Alias for a read-only SysExMsgReference. */
using SysExMsgView = SysExMsgReference<MsgAccess::kReadOnly>;

/** Alias for a read-write SysExMsgReference. */
using SysExMsgRef = SysExMsgReference<MsgAccess::kReadWrite>;

/** Alias for a timestamped read-only reference to a SysEx message. */
using TimedSysExMsgView = Timed<SysExMsgView>;

/** Alias for a timestamped read-write reference to a SysEx message. */
using TimedSysExMsgRef = Timed<SysExMsgRef>;

/**
 * A reference to a manufacturer-specific (or private non-commercial) System
 * Exclusive (SysEx) message stored as contiguous bytes (which must outlive
 * this reference object), including a terminating End of Exclusive (EOX) status
 * byte.
 *
 * Can either be read-write (see MfrSysExMsgRef alias) or read-only (see
 * MfrSysExMsgView alias), based on AccessType template parameter.
 */
template<MsgAccess AccessType>
class MfrSysExMsgReference : public SysExMsgReference<AccessType> {
public:
  using BytePointerType = typename SysExMsgReference<AccessType>::BytePointerType;

  explicit MfrSysExMsgReference(BytePointerType bytes, int numBytes)
      : SysExMsgReference<AccessType>{bytes, numBytes} {
    assert(!this->isUniversal());
  }

  /** Returns the Manufacturer this SysEx message is for. */
  Manufacturer manufacturer() const {
    if (this->sysExId() == Manufacturer::kExtendedSysExId) {
      return Manufacturer::extId(this->rawBytes()[2], this->rawBytes()[3]);
    } else if (this->sysExId() == Manufacturer::kNonCommercialSysExId) {
      return Manufacturer::nonCommercial();
    } else {
      return Manufacturer::shortId(this->sysExId());
    }
  }

  // NOTE: No mutation provided for setting Manufacturer, since that could
  // change the # of bytes this message requires.

  /**
   * Returns read-only pointer to span of raw payload bytes, after the 1 or 3
   * byte manufacturer ID (and not including the terminating EOX byte).
   *
   * See numPayloadBytes() for size.
   */
  const std::uint8_t* rawPayloadBytes() const {
    return &(this->rawBytes()[numHeaderBytes()]);
  }

  /**
   * Returns read-write pointer to span of raw payload bytes, after the 1 or 3
   * byte manufacturer ID (and not including the terminating EOX byte).
   *
   * See numPayloadBytes() for size.
   */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  std::uint8_t* rawPayloadBytes() {
    return &(this->rawBytes()[numHeaderBytes()]);
  }

  /**
   * Returns the # of payload bytes accessible through rawPayloadBytes(),
   * starting after the 1 or 3 bytes manufacturer ID (and not including the
   * terminating EOX byte).
   */
  int numPayloadBytes() const {
    return this->numBytes() - numHeaderBytes() - 1;  // Last -1 for trailing EOX.
  }

private:
  int numHeaderBytes() const {
    const auto numMfrBytes = (this->sysExId() == Manufacturer::kExtendedSysExId)
        ? internal::kNumManufacturerIdExtBytes
        : internal::kNumManufacturerIdShortBytes;
    return internal::kOneSysExHdrStatusByte + numMfrBytes;
  }
};

/** Alias for a read-only MfrSysExMsgReference. */
using MfrSysExMsgView = MfrSysExMsgReference<MsgAccess::kReadOnly>;

/** Alias for a read-write MfrSysExMsgReference. */
using MfrSysExMsgRef = MfrSysExMsgReference<MsgAccess::kReadWrite>;

/** Alias for a timestamped read-only reference to a manufacturer-specific SysEx message. */
using TimedMfrSysExMsgView = Timed<MfrSysExMsgView>;

/** Alias for a timestamped read-write reference to a manufacturer-specific SysEx message. */
using TimedMfrSysExMsgRef = Timed<MfrSysExMsgRef>;

/**
 * A reference to a Non-Realtime or Realtime Universal System Exclusive (SysEx)
 * message stored as contiguous bytes (which must outlive this reference object),
 * including a terminating End of Exclusive (EOX) status byte.
 *
 * Can either be read-write (see UniversalSysExMsgRef alias) or read-only (see
 * UniversalSysExMsgView alias), based on AccessType template parameter.
 */
template<MsgAccess AccessType>
class UniversalSysExMsgReference : public SysExMsgReference<AccessType> {
public:
  using BytePointerType = typename SysExMsgReference<AccessType>::BytePointerType;

  explicit UniversalSysExMsgReference(BytePointerType bytes, int numBytes)
      : SysExMsgReference<AccessType>{bytes, numBytes} {
    assert(this->isUniversal());
  }

  /** Returns the category of universal message this is (non-realtime or realtime). */
  UniversalCategory category() const {
    return static_cast<UniversalCategory>(this->sysExId());
  }

  /** Returns the type of universal message this is. */
  UniversalType universalType() const {
    const auto cat = category();
    const auto subId1 = this->rawBytes()[3];

    if (cat == UniversalCategory::kNonRealTime) {
      return internal::typeHasSubId2(cat, subId1)
          ? UniversalType::nonRealTime(subId1, this->rawBytes()[4])
          : UniversalType::nonRealTime(subId1);
    } else {
      return internal::typeHasSubId2(cat, subId1)
          ? UniversalType::realTime(subId1, this->rawBytes()[4])
          : UniversalType::realTime(subId1);
    }
  }

  // NOTE: No mutations provided for category or type, since those could change
  // the # of bytes this message requires.

  /**
   * Returns the device that should respond to this message (or special "all"
   * value that indicates message is for all receiving devices).
   */
  Device device() const {
    return (this->rawBytes()[2] == Device::all().value())
        ? Device::all()
        : Device::id(this->rawBytes()[2]);
  }

  /** Sets the device (or special "all" value) that should respond. */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setDevice(Device device) {
    this->rawBytes()[2] = device.value();
  }

  /**
   * Returns read-only pointer to span of raw payload bytes, after the SysEx ID,
   * device ID, and 1 or 2 sub-ID bytes (and not including the terminating EOX
   * byte).
   *
   * See numPayloadBytes() for size.
   */
  const std::uint8_t* rawPayloadBytes() const {
    return &(this->rawBytes()[numHeaderBytes()]);
  }

  /**
   * Returns read-write pointer to span of raw payload bytes, after the SysEx
   * ID, device ID, and 1 or 2 sub-ID bytes (and not including the terminating
   * EOX byte).
   *
   * See numPayloadBytes() for size.
   */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  std::uint8_t* rawPayloadBytes() {
    return &(this->rawBytes()[numHeaderBytes()]);
  }

  /**
   * Returns the # of payload bytes accessible through rawPayloadBytes(),
   * starting after the SysEx ID, device ID, and 1 or 2 sub-ID bytes (and not
   * including the terminating EOX byte).
   */
  int numPayloadBytes() const {
    return this->numBytes() - numHeaderBytes() - 1;  // Last -1 for trailing EOX.
  }

private:
  int numHeaderBytes() const {
    const auto numUniversalHdrBytes = internal::typeHasSubId2(category(), this->rawBytes()[3])
        ? internal::kNumUniversalHdrBytesTwoSubIds
        : internal::kNumUniversalHdrBytesOneSubId;
    return internal::kOneSysExHdrStatusByte + numUniversalHdrBytes;
  }
};

/** Alias for a read-only UniversalSysExMsgReference. */
using UniversalSysExMsgView = UniversalSysExMsgReference<MsgAccess::kReadOnly>;

/** Alias for a read-write UniversalSysExMsgReference. */
using UniversalSysExMsgRef = UniversalSysExMsgReference<MsgAccess::kReadWrite>;

/** Alias for a timestamped read-only reference to a manufacturer-specific SysEx message. */
using TimedUniversalSysExMsgView = Timed<UniversalSysExMsgView>;

/** Alias for a timestamped read-write reference to a manufacturer-specific SysEx message. */
using TimedUniversalSysExMsgRef = Timed<UniversalSysExMsgRef>;

/**
 * A reference to an MTC Quarter Frame message stored as contiguous bytes (which
 * must outlive this reference object). Can either be read-write (see
 * MtcQuarterFrameMsgRef alias) or read-only (see MtcQuarterFrameMsgView alias),
 * based on AccessType template parameter.
 */
template<MsgAccess AccessType>
class MtcQuarterFrameMsgReference : public MsgReference<AccessType> {
public:
  using BytePointerType = typename MsgReference<AccessType>::BytePointerType;

  explicit MtcQuarterFrameMsgReference(BytePointerType bytes, int numBytes)
      : MsgReference<AccessType>{bytes, numBytes} {
    assert(this->type() == MsgType::kMtcQuarterFrame);
  }

  /**
   * Returns full 7-bit data byte value, which can be used with
   * MtcFullFrame::setPieceFromQuarterFrameDataByte().
   *
   * Encoded as bits 0b0nnn'dddd, where nnn denotes which quarter frame piece
   * his is (see MtcQuarterFramePiece) and dddd encodes the data value for that
   * piece.
   */
  std::uint8_t dataByte() const { return this->rawBytes()[1]; }

  /**
   * Sets full 7-bit data byte value, e.g. as obtained from
   * MtcFullFrame::quarterFrameDataByteFor().
   *
   * Should be encoded as bits 0b0nnn'dddd, where nnn denotes which quarter
   * frame piece this is (see MtcQuarterFramePiece) and dddd encodes the data
   * value for that piece.
   */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setDataByte(std::uint8_t dataByte) {
    assert(dataByte <= static_cast<int>(DataValue::max().value()));
    this->rawBytes()[1] = dataByte;
  }

  /**
   * Returns the piece of the full timecode value that this quarter frame
   * message provides a value for.
   */
  MtcQuarterFramePiece piece() const {
    return static_cast<MtcQuarterFramePiece>(
        internal::getBits(dataByte(), internal::kMtcQuarterFramePieceBits));
  }

  /**
   * Sets the piece of the full timecode value that this quarter frame message
   * provides a value for.
   */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setPiece(MtcQuarterFramePiece piece) {
    setDataByte(internal::setBits(dataByte(), static_cast<std::uint8_t>(piece),
                internal::kMtcQuarterFramePieceBits));
  }

  /**
   * Provides the value for the piece of timecode this quarter frame message is
   * setting, encoded in the lower 4 bits of the returned value (the upper 4
   * bits will always be 0s).
   */
  std::uint8_t valueInLower4Bits() const {
    return internal::getBits(dataByte(), internal::kMtcQuarterFrameValueBits);
  }

  /**
   * Sets the value for the piece of timecode this quarter frame message is
   * settings, encoded in the lower 4 bits of the given value (the upper 4
   * bits must be 0s).
   */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setValueFromLower4Bits(std::uint8_t value) {
    assert(value <= 0b0000'1111);
    setDataByte(internal::setBits(dataByte(), value, internal::kMtcQuarterFrameValueBits));
  }
};

/** Alias for a read-only MtcQuarterFrameMsgReference. */
using MtcQuarterFrameMsgView = MtcQuarterFrameMsgReference<MsgAccess::kReadOnly>;

/** Alias for a read-write MtcQuarterFrameMsgReference. */
using MtcQuarterFrameMsgRef = MtcQuarterFrameMsgReference<MsgAccess::kReadWrite>;

/** Alias for a timestamped read-only reference to an MTC Quarter Frame message. */
using TimedMtcQuarterFrameMsgView = Timed<MtcQuarterFrameMsgView>;

/** Alias for a timestamped read-write reference to an MTC Quarter Frame message. */
using TimedMtcQuarterFrameMsgRef = Timed<MtcQuarterFrameMsgRef>;

/**
 * A reference to a Song Position Pointer message stored as contiguous bytes
 * (which must outlive this reference object). Can either be read-write (see
 * SongPosMsgRef alias) or read-only (see SongPosMsgView alias), based on
 * AccessType template parameter.
 */
template<MsgAccess AccessType>
class SongPosMsgReference : public MsgReference<AccessType> {
public:
  using BytePointerType = typename MsgReference<AccessType>::BytePointerType;

  explicit SongPosMsgReference(BytePointerType bytes, int numBytes)
      : MsgReference<AccessType>{bytes, numBytes} {
    assert(this->type() == MsgType::kSongPositionPointer);
  }

  /**
   * Returns 14-bit [0, 16383] song position, measured as the number of sixteenth
   * notes after the start of the song.
   *
   * Regardless of a sequencer's internal pulses per quarter note (PPQ), there
   * are 24 "MIDI clock" pulses per quarter note, so this song position is
   * measured in multiples of 6 MIDI clocks = 1 sixteenth note.
   */
  DoubleDataValue sixteenthsAfterStart() const {
    return DoubleDataValue::fromLsbMsb(
        static_cast<std::uint8_t>(this->data1().value()),
        static_cast<std::uint8_t>(this->data2().value()));
  }

  /**
   * Updates to the given 14-bit [0, 16383] song position, measured as the
   * number of sixteenth notes after the start of the song.
   *
   * Regardless of a sequencer's internal pulses per quarter note (PPQ), there
   * are 24 "MIDI clock" pulses per quarter note, so this song position is
   * measured in multiples of 6 MIDI clocks = 1 sixteenth note.
   */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setSixteenthsAfterStart(DoubleDataValue soxteenthsAfterStart) {
    this->setData1(DataValue{static_cast<std::int8_t>(soxteenthsAfterStart.lsb())});
    this->setData2(DataValue{static_cast<std::int8_t>(soxteenthsAfterStart.msb())});
  }
};

/** Alias for a read-only SongPosMsgReference. */
using SongPosMsgView = SongPosMsgReference<MsgAccess::kReadOnly>;

/** Alias for a read-write SongPosMsgReference. */
using SongPosMsgRef = SongPosMsgReference<MsgAccess::kReadWrite>;

/** Alias for a timestamped read-only reference to a song position message. */
using TimedSongPosMsgView = Timed<SongPosMsgView>;

/** Alias for a timestamped read-write reference to a song position message. */
using TimedSongPosMsgRef = Timed<SongPosMsgRef>;

/**
 * A reference to a Song Select message stored as contiguous bytes (which
 * must outlive this reference object). Can either be read-write (see
 * SongSelectMsgRef alias) or read-only (see SongSelectMsgView alias),
 * based on AccessType template parameter.
 */
template<MsgAccess AccessType>
class SongSelectMsgReference : public MsgReference<AccessType> {
public:
  using BytePointerType = typename MsgReference<AccessType>::BytePointerType;

  explicit SongSelectMsgReference(BytePointerType bytes, int numBytes)
      : MsgReference<AccessType>{bytes, numBytes} {
    assert(this->type() == MsgType::kSongSelect);
  }

  /** Returns song number. */
  PresetNumber song() const { return PresetNumber::index(this->data1().value()); }

  /**
   * Updates to the given song number, which must be in normal [0, 127] index
   * range (not a special "none" value).
   */
  template<
      MsgAccess AccessT = AccessType,
      typename = std::enable_if_t<AccessT == MsgAccess::kReadWrite>>
  void setSong(PresetNumber song) {
    assert(song.isNormal());
    this->setData1(DataValue{static_cast<std::int8_t>(song.index())});
  }
};

/** Alias for a read-only SongSelectMsgReference. */
using SongSelectMsgView = SongSelectMsgReference<MsgAccess::kReadOnly>;

/** Alias for a read-write SongSelectMsgReference. */
using SongSelectMsgRef = SongSelectMsgReference<MsgAccess::kReadWrite>;

/** Alias for a timestamped read-only reference to a song select message. */
using TimedSongSelectMsgView = Timed<SongSelectMsgView>;

/** Alias for a timestamped read-write reference to a song select message. */
using TimedSongSelectMsgRef = Timed<SongSelectMsgRef>;

/**
 * A reference to an Oscillator Tune Request message stored as contiguous bytes
 * (which must outlive this reference object). Can either be read-write (see
 * OscTuneMsgRef alias) or read-only (see OscTuneMsgView alias), based on
 * AccessType template parameter.
 */
template<MsgAccess AccessType>
class OscTuneMsgReference : public MsgReference<AccessType> {
public:
  using BytePointerType = typename MsgReference<AccessType>::BytePointerType;

  explicit OscTuneMsgReference(BytePointerType bytes, int numBytes)
      : MsgReference<AccessType>{bytes, numBytes} {
    assert(this->type() == MsgType::kOscillatorTuneRequest);
  }
};

/** Alias for a read-only OscTuneMsgReference. */
using OscTuneMsgView = OscTuneMsgReference<MsgAccess::kReadOnly>;

/** Alias for a read-write OscTuneMsgReference. */
using OscTuneMsgRef = OscTuneMsgReference<MsgAccess::kReadWrite>;

/** Alias for a timestamped read-only reference to an oscillator tune request message. */
using TimedOscTuneMsgView = Timed<OscTuneMsgView>;

/** Alias for a timestamped read-write reference to an oscillator tune request message. */
using TimedOscTuneMsgRef = Timed<OscTuneMsgRef>;

}  // namespace bmmidi

#endif  // BMMIDI_MSG_REFERENCE_HPP
