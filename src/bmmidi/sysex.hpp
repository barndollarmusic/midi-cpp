// SPDX-FileCopyrightText: 2022 Barndollar Music, Ltd.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef BMMIDI_SYSEX_HPP
#define BMMIDI_SYSEX_HPP

#include <cassert>
#include <cstdint>
#include <memory>

#include "bmmidi/cpp_features.hpp"

namespace bmmidi {

/**
 * ID for a registered MIDI manufacturer, which is either a 1-byte short ID
 * or a 2-byte extended ID. Extended IDs are in a different ID space from
 * short IDs (so the value 0x01 as a short ID is distinct from the value
 * 0x00 0x01 as an extended ID).
 *
 * Also includes a special non-commercial value for private use by schools,
 * research projects, etc.
 *
 * See https://www.midi.org/specifications-old/item/manufacturer-id-numbers.
 */
class Manufacturer {
public:
  /** SysEx ID byte denoting an extended multi-byte manufacturer ID. */
  static constexpr std::uint8_t kExtendedSysExId = 0x00;

  /** SysEx ID byte for a non-commercial manufacturer (for private use). */
  static constexpr std::uint8_t kNonCommercialSysExId = 0x7D;

  /**
   * Returns Manufacturer with given short 1-byte ID. For example, Sequential
   * Circuits has the short ID 0x01.
   */
  static constexpr Manufacturer shortId(std::uint8_t shortMfrId) {
    assert((0x01 <= shortMfrId) && (shortMfrId < kNonCommercialSysExId));
    return Manufacturer{shortMfrId, 0x00, 0x00};
  }

  /**
   * Returns Manufacturer with given extended 2-byte ID (not including the leading 0x00
   * SysEx ID).
   *
   * For example, for Spectrasonics extByte1 would be 0x02 and extByte2 would be 0x2C.
   */
  static constexpr Manufacturer extId(std::uint8_t extByte1, std::uint8_t extByte2) {
    assert(extByte1 <= 0x7F);
    assert(extByte2 <= 0x7F);
    return Manufacturer{kExtendedSysExId, extByte1, extByte2};
  }

  /**
   * Returns generic non-commercial Manufacturer that can be used privately by
   * schools, research projects, etc.
   */
  static constexpr Manufacturer nonCommercial() {
    return Manufacturer{kNonCommercialSysExId, 0x00, 0x00};
  }

  /** Returns true if this is an extended 2-byte manufacturer ID. */
  constexpr bool isExtended() const { return (sysExId_ == kExtendedSysExId); }

  /**
   * Returns SysEx ID, which will be the short 1-byte manufacturer ID or
   * kExtendedSysExId (0x00) if this is an extended manufacturer ID.
   */
  constexpr std::uint8_t sysExId() const { return sysExId_; }

  /**
   * Returns 1st byte of extended manufacturer ID (not including leading 0x00 of
   * the SysEx Sub-ID). Only valid if isExtended().
   */
  constexpr std::uint8_t extByte1() const {
    assert(isExtended());
    return extByte1_;
  }

  /**
   * Returns 1st byte of extended manufacturer ID (not including leading 0x00 of
   * the SysEx Sub-ID). Only valid if isExtended().
   */
  constexpr std::uint8_t extByte2() const {
    assert(isExtended());
    return extByte2_;
  }

  // Equality operations:
  friend constexpr bool operator==(Manufacturer lhs, Manufacturer rhs) {
    return (lhs.sysExId_ == rhs.sysExId_)
        && (lhs.extByte1_ == rhs.extByte1_)
        && (lhs.extByte2_ == rhs.extByte2_);
  }

  friend constexpr bool operator!=(Manufacturer lhs, Manufacturer rhs) { return !(lhs == rhs); }

private:
  constexpr explicit Manufacturer(
      std::uint8_t sysExId, std::uint8_t extByte1, std::uint8_t extByte2)
      : sysExId_{sysExId}, extByte1_{extByte1}, extByte2_{extByte2} {}

  std::uint8_t sysExId_;
  std::uint8_t extByte1_;
  std::uint8_t extByte2_;
};

/**
 * Category of Universal System Exclusive messages (Real Time or Non-Real Time).
 *
 * This serves as the SysEx ID number (first data byte after the F0 status byte).
 */
enum class UniversalCategory : std::uint8_t {
  kNonRealTime = 0x7E,
  kRealTime = 0x7F,
};

/**
 * Represents the type of Universal SysEx message, with the SysEx ID (Non-Realtime or Realtime)
 * and 1 or 2 Sub-IDs.
 *
 * See bmmidi::universal namespace for full list of constants defined by the MIDI standard.
 */
class UniversalType {
public:
  /** Creates a Non-Realtime UniversalType. */
  static constexpr UniversalType nonRealTime(
      std::uint8_t subId1, std::uint8_t subId2 = kUnusedSubId) {
    return UniversalType{UniversalCategory::kNonRealTime, subId1, subId2};
  }

  /** Creates a Realtime UniversalType. */
  static constexpr UniversalType realTime(
      std::uint8_t subId1, std::uint8_t subId2 = kUnusedSubId) {
    return UniversalType{UniversalCategory::kRealTime, subId1, subId2};
  }

  /** Returns whether this is a non-realtime or realtime type. */
  constexpr UniversalCategory category() const { return category_; }

  /**
   * Returns sub-ID #1 that identifies this type of universal SysEx message
   * (within its non-realtime or realtime category).
   */
  constexpr std::uint8_t subId1() const { return subId1_; }

  /** Returns true if this type has a Sub-ID #2. */
  constexpr bool hasSubId2() const { return (subId2_ != kUnusedSubId); }

  /**
   * Returns sub-ID #2 that further distinguishes this message type within its
   * sub-ID #1. Only valid if hasSubId2().
   */
  constexpr std::uint8_t subId2() const {
    assert(hasSubId2());
    return subId2_;
  }

  // Equality operations:
  friend constexpr bool operator==(UniversalType lhs, UniversalType rhs) {
    return (lhs.category_ == rhs.category_)
        && (lhs.subId1_ == rhs.subId1_)
        && (lhs.subId2_ == rhs.subId2_);
  }

  friend constexpr bool operator!=(UniversalType lhs, UniversalType rhs) { return !(lhs == rhs); }

private:
  static constexpr std::uint8_t kUnusedSubId = 0xFF;

  explicit constexpr UniversalType(
      UniversalCategory category, std::uint8_t subId1, std::uint8_t subId2)
      : category_{category}, subId1_{subId1}, subId2_{subId2} {
    assert(subId1 <= 0x7F);
    assert((subId2 <= 0x7F) || (subId2 == kUnusedSubId));
  }

  UniversalCategory category_;
  std::uint8_t subId1_;
  std::uint8_t subId2_;
};

namespace universal {

//==============================================================================
// Non-Realtime (0x7E)
//==============================================================================

/** Non-Realtime Sample Dump Header. */
BMMIDI_INLINE_VAR constexpr UniversalType kSampleDumpHeader = UniversalType::nonRealTime(0x01);

/** Non-Realtime Sample Data Packet. */
BMMIDI_INLINE_VAR constexpr UniversalType kSampleDataPacket = UniversalType::nonRealTime(0x02);

/** Non-Realtime Sample Dump Request. */
BMMIDI_INLINE_VAR constexpr UniversalType kSampleDumpRequest = UniversalType::nonRealTime(0x03);

/** Non-Realtime MIDI Time Code (MTC): Special. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcNonRtSpecial = UniversalType::nonRealTime(0x04, 0x00);

/** Non-Realtime MIDI Time Code (MTC): Punch In Points. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcNonRtPunchInPoints = UniversalType::nonRealTime(0x04, 0x01);

/** Non-Realtime MIDI Time Code (MTC): Punch Out Points. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcNonRtPunchOutPoints = UniversalType::nonRealTime(0x04, 0x02);

/** Non-Realtime MIDI Time Code (MTC): Delete Punch In Point. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcNonRtDeletePunchInPoint = UniversalType::nonRealTime(0x04, 0x03);

/** Non-Realtime MIDI Time Code (MTC): Delete Punch Out Point. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcNonRtDeletePunchOutPoint = UniversalType::nonRealTime(0x04, 0x04);

/** Non-Realtime MIDI Time Code (MTC): Event Start Point. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcNonRtEventStartPoint = UniversalType::nonRealTime(0x04, 0x05);

/** Non-Realtime MIDI Time Code (MTC): Event Stop Point. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcNonRtEventStopPoint = UniversalType::nonRealTime(0x04, 0x06);

/** Non-Realtime MIDI Time Code (MTC): Event Start Points with Additional Info. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcNonRtEventStartsAddlInfo = UniversalType::nonRealTime(0x04, 0x07);

/** Non-Realtime MIDI Time Code (MTC): Event Stop Points with Additional Info. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcNonRtEventStopsAddlInfo = UniversalType::nonRealTime(0x04, 0x08);

/** Non-Realtime MIDI Time Code (MTC): Delete Event Start Point. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcNonRtDeleteEventStartPoint = UniversalType::nonRealTime(0x04, 0x09);

/** Non-Realtime MIDI Time Code (MTC): Delete Event Stop Point. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcNonRtDeleteEventStopPoint = UniversalType::nonRealTime(0x04, 0x0A);

/** Non-Realtime MIDI Time Code (MTC): Cue Points. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcNonRtCuePoints = UniversalType::nonRealTime(0x04, 0x0B);

/** Non-Realtime MIDI Time Code (MTC): Cue Points with Additional Info. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcNonRtCuePointsAddlInfo = UniversalType::nonRealTime(0x04, 0x0C);

/** Non-Realtime MIDI Time Code (MTC): Delete Cue Point. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcNonRtDeleteCuePoint = UniversalType::nonRealTime(0x04, 0x0D);

/** Non-Realtime MIDI Time Code (MTC): Event Name in Additional Info. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcNonRtEventNameAddlInfo = UniversalType::nonRealTime(0x04, 0x0E);

/** Non-Realtime Sample Dump: Loop Points Transmission. */
BMMIDI_INLINE_VAR constexpr UniversalType kSampleDumpLoopPointsTrans = UniversalType::nonRealTime(0x05, 0x01);

/** Non-Realtime Sample Dump: Loop Points Request. */
BMMIDI_INLINE_VAR constexpr UniversalType kSampleDumpLoopPointsReq = UniversalType::nonRealTime(0x05, 0x02);

/** Non-Realtime Sample Dump: Sample Name Transmission. */
BMMIDI_INLINE_VAR constexpr UniversalType kSampleDumpSampleNameTrans = UniversalType::nonRealTime(0x05, 0x03);

/** Non-Realtime Sample Dump: Sample Name Request. */
BMMIDI_INLINE_VAR constexpr UniversalType kSampleDumpSampleNameReq = UniversalType::nonRealTime(0x05, 0x04);

/** Non-Realtime Sample Dump: Extended Dump Header. */
BMMIDI_INLINE_VAR constexpr UniversalType kSampleDumpExtHeader = UniversalType::nonRealTime(0x05, 0x05);

/** Non-Realtime Sample Dump: Extended Loop Points Transmission. */
BMMIDI_INLINE_VAR constexpr UniversalType kSampleDumpExtLoopPointsTrans = UniversalType::nonRealTime(0x05, 0x06);

/** Non-Realtime Sample Dump: Extended Loop Points Request. */
BMMIDI_INLINE_VAR constexpr UniversalType kSampleDumpExtLoopPointsReq = UniversalType::nonRealTime(0x05, 0x07);

/** Non-Realtime General Information: Identity Request. */
BMMIDI_INLINE_VAR constexpr UniversalType kIdentityReq = UniversalType::nonRealTime(0x06, 0x01);

/** Non-Realtime General Information: Identity Reply. */
BMMIDI_INLINE_VAR constexpr UniversalType kIdentityReply = UniversalType::nonRealTime(0x06, 0x02);

/** Non-Realtime File Dump: Header. */
BMMIDI_INLINE_VAR constexpr UniversalType kFileDumpHeader = UniversalType::nonRealTime(0x07, 0x01);

/** Non-Realtime File Dump: Data Packet. */
BMMIDI_INLINE_VAR constexpr UniversalType kFileDumpData = UniversalType::nonRealTime(0x07, 0x02);

/** Non-Realtime File Dump: Request. */
BMMIDI_INLINE_VAR constexpr UniversalType kFileDumpReq = UniversalType::nonRealTime(0x07, 0x03);

/** Non-Realtime MIDI Tuning: Bulk Dump Request. */
BMMIDI_INLINE_VAR constexpr UniversalType kTuningBulkDumpReq = UniversalType::nonRealTime(0x08, 0x00);

/** Non-Realtime MIDI Tuning: Bulk Dump Reply. */
BMMIDI_INLINE_VAR constexpr UniversalType kTuningBulkDumpReply = UniversalType::nonRealTime(0x08, 0x01);

/** Non-Realtime MIDI Tuning: Dump Request. */
BMMIDI_INLINE_VAR constexpr UniversalType kTuningDumpReq = UniversalType::nonRealTime(0x08, 0x03);

/** Non-Realtime MIDI Tuning: Key-Based Tuning Dump. */
BMMIDI_INLINE_VAR constexpr UniversalType kTuningKeyBasedDump = UniversalType::nonRealTime(0x08, 0x04);

/** Non-Realtime MIDI Tuning: Scale/Octave Tuning Dump, 1 byte format. */
BMMIDI_INLINE_VAR constexpr UniversalType kTuningNonRtOctDump1B = UniversalType::nonRealTime(0x08, 0x05);

/** Non-Realtime MIDI Tuning: Scale/Octave Tuning Dump, 2 byte format. */
BMMIDI_INLINE_VAR constexpr UniversalType kTuningNonRtOctDump2B = UniversalType::nonRealTime(0x08, 0x06);

/** Non-Realtime MIDI Tuning: Single Note Tuning Change with Bank Select. */
BMMIDI_INLINE_VAR constexpr UniversalType kTuningNonRtNoteChangeBank = UniversalType::nonRealTime(0x08, 0x07);

/** Non-Realtime MIDI Tuning: Scale/Octave Tuning, 1 byte format. */
BMMIDI_INLINE_VAR constexpr UniversalType kTuningNonRtOct1B = UniversalType::nonRealTime(0x08, 0x08);

/** Non-Realtime MIDI Tuning: Scale/Octave Tuning, 2 byte format. */
BMMIDI_INLINE_VAR constexpr UniversalType kTuningNonRtOct2B = UniversalType::nonRealTime(0x08, 0x09);

/** Non-Realtime General MIDI: GM1 System On. */
BMMIDI_INLINE_VAR constexpr UniversalType kGeneralMidi1SysOn = UniversalType::nonRealTime(0x09, 0x01);

/** Non-Realtime General MIDI: System Off. */
BMMIDI_INLINE_VAR constexpr UniversalType kGeneralMidiSysOff = UniversalType::nonRealTime(0x09, 0x02);

/** Non-Realtime General MIDI: GM2 System On. */
BMMIDI_INLINE_VAR constexpr UniversalType kGeneralMidi2SysOn = UniversalType::nonRealTime(0x09, 0x03);

/** Non-Realtime Downloadable Sounds (DLS): Turn On. */
BMMIDI_INLINE_VAR constexpr UniversalType kDlsTurnOn = UniversalType::nonRealTime(0x0A, 0x01);

/** Non-Realtime Downloadable Sounds (DLS): Turn Off. */
BMMIDI_INLINE_VAR constexpr UniversalType kDlsTurnOff = UniversalType::nonRealTime(0x0A, 0x02);

/** Non-Realtime Downloadable Sounds (DLS): Turn Voice Allocation Off. */
BMMIDI_INLINE_VAR constexpr UniversalType kDlsVoiceAllocOff = UniversalType::nonRealTime(0x0A, 0x03);

/** Non-Realtime Downloadable Sounds (DLS): Turn Voice Allocation On. */
BMMIDI_INLINE_VAR constexpr UniversalType kDlsVoiceAllocOn = UniversalType::nonRealTime(0x0A, 0x04);

/** Non-Realtime File Reference Message: Open File. */
BMMIDI_INLINE_VAR constexpr UniversalType kFileRefOpen = UniversalType::nonRealTime(0x0B, 0x01);

/** Non-Realtime File Reference Message: Select or Reselect Contents. */
BMMIDI_INLINE_VAR constexpr UniversalType kFileRefSelect = UniversalType::nonRealTime(0x0B, 0x02);

/** Non-Realtime File Reference Message: Open File and Select Contents. */
BMMIDI_INLINE_VAR constexpr UniversalType kFileRefOpenSelect = UniversalType::nonRealTime(0x0B, 0x03);

/** Non-Realtime File Reference Message: Close File. */
BMMIDI_INLINE_VAR constexpr UniversalType kFileRefClose = UniversalType::nonRealTime(0x0B, 0x04);

/** Non-Realtime MIDI Visual Control (MVC): Command Set 1. */
BMMIDI_INLINE_VAR constexpr UniversalType kMvcCmdSet1 = UniversalType::nonRealTime(0x0C, 0x01);

// NOTE: Not including MIDI 2.0 Capability Inquiry here yet.

/** Non-Realtime End of File. */
BMMIDI_INLINE_VAR constexpr UniversalType kEof = UniversalType::nonRealTime(0x7B);

/** Non-Realtime Wait (requests pausing of data packet transmission until next ACK). */
BMMIDI_INLINE_VAR constexpr UniversalType kWait = UniversalType::nonRealTime(0x7C);

/** Non-Realtime Cancel (cancels request for data packet transmission). */
BMMIDI_INLINE_VAR constexpr UniversalType kCancel = UniversalType::nonRealTime(0x7D);

/** Non-Realtime NAK (requests retransmission of data packet). */
BMMIDI_INLINE_VAR constexpr UniversalType kNak = UniversalType::nonRealTime(0x7E);

/** Non-Realtime ACK (acknowledges receipt of data packet). */
BMMIDI_INLINE_VAR constexpr UniversalType kAck = UniversalType::nonRealTime(0x7F);

//==============================================================================
// Realtime (0x7F)
//==============================================================================

/** Realtime MIDI Time Code (MTC) Full Message. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcFull = UniversalType::realTime(0x01, 0x01);

/** Realtime MIDI Time Code (MTC) User Bits. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcUserBits = UniversalType::realTime(0x01, 0x02);

// NOTE: Not including MIDI Show Control (MSC) commands.

/** Realtime Notation Information: Bar Number. */
BMMIDI_INLINE_VAR constexpr UniversalType kNotationBarNum = UniversalType::realTime(0x03, 0x01);

/** Realtime Notation Information: Time Signature (Immediate). */
BMMIDI_INLINE_VAR constexpr UniversalType kNotationTimeSigImmediate = UniversalType::realTime(0x03, 0x02);

/** Realtime Notation Information: Time Signature (Delayed). */
BMMIDI_INLINE_VAR constexpr UniversalType kNotationTimeSigDelayed = UniversalType::realTime(0x03, 0x42);

/** Realtime MIDI Time Code (MTC): Special. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcRtSpecial = UniversalType::realTime(0x05, 0x00);

/** Realtime MIDI Time Code (MTC): Punch In Points. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcRtPunchInPoints = UniversalType::realTime(0x05, 0x01);

/** Realtime MIDI Time Code (MTC): Punch Out Points. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcRtPunchOutPoints = UniversalType::realTime(0x05, 0x02);

/** Realtime MIDI Time Code (MTC): Event Start Points. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcRtEventStartPoints = UniversalType::realTime(0x05, 0x05);

/** Realtime MIDI Time Code (MTC): Event Stop Points. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcRtEventStopPoints = UniversalType::realTime(0x05, 0x06);

/** Realtime MIDI Time Code (MTC): Event Start Points with Additional Info. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcRtEventStartsAddlInfo = UniversalType::realTime(0x05, 0x07);

/** Realtime MIDI Time Code (MTC): Event Stop Points with Additional Info. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcRtEventStopsAddlInfo = UniversalType::realTime(0x05, 0x08);

/** Realtime MIDI Time Code (MTC): Cue Points. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcRtCuePoints = UniversalType::realTime(0x05, 0x0B);

/** Realtime MIDI Time Code (MTC): Cue Points with Additional Info. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcRtCuePointsAddlInfo = UniversalType::realTime(0x05, 0x0C);

/** Realtime MIDI Time Code (MTC): Event Name in Additional Info. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcRtEventNameAddlInfo = UniversalType::realTime(0x05, 0x0E);

// NOTE: Not including MIDI Machine Control (MMC) commands or responses.

/** Realtime MIDI Tuning: Single Note Tuning Change. */
BMMIDI_INLINE_VAR constexpr UniversalType kTuningRtNoteChange = UniversalType::realTime(0x08, 0x02);

/** Realtime MIDI Tuning: Single Note Tuning Change with Bank Select. */
BMMIDI_INLINE_VAR constexpr UniversalType kTuningRtNoteChangeBank = UniversalType::realTime(0x08, 0x07);

/** Realtime MIDI Tuning: Scale/Octave Tuning, 1 byte format. */
BMMIDI_INLINE_VAR constexpr UniversalType kTuningRtOct1B = UniversalType::realTime(0x08, 0x08);

/** Realtime MIDI Tuning: Scale/Octave Tuning, 2 byte format. */
BMMIDI_INLINE_VAR constexpr UniversalType kTuningRtOct2B = UniversalType::realTime(0x08, 0x09);

/** Realtime Controller Destination Setting: Channel Pressure (Aftertouch). */
BMMIDI_INLINE_VAR constexpr UniversalType kControlDestChanPressure = UniversalType::realTime(0x09, 0x01);

/** Realtime Controller Destination Setting: Polyphonic Key Pressure (Per-Key Aftertouch). */
BMMIDI_INLINE_VAR constexpr UniversalType kControlDestPolyKeyPressure = UniversalType::realTime(0x09, 0x02);

/** Realtime Controller Destination Setting: Controller (Control Change). */
BMMIDI_INLINE_VAR constexpr UniversalType kControlDestCC = UniversalType::realTime(0x09, 0x03);

/** Realtime Key-Based Instrument Control. */
BMMIDI_INLINE_VAR constexpr UniversalType kKeyBasedInstrControl = UniversalType::realTime(0x0A, 0x01);

/** Realtime Scalable Polyphony MIDI MIP Message. */
BMMIDI_INLINE_VAR constexpr UniversalType kScalablePolyMip = UniversalType::realTime(0x0B, 0x01);

/** Realtime Mobile Phone Control Message. */
BMMIDI_INLINE_VAR constexpr UniversalType kMobilePhoneControl = UniversalType::realTime(0x0C, 0x00);

}  // namespace universal

/**
 * A [0, 127] identifier that can be used to address a specific device (or all
 * devices) in System Exclusive messages.
 */
class Device {
public:
  /** Returns special "all call" Device that all devices should respond to. */
  static constexpr Device all() { return Device{kAll}; }

  /** Returns [0, 126] identifier for a specific device. */
  static constexpr Device id(std::uint8_t deviceId) {
    assert(deviceId != kAll);
    return Device(deviceId);
  }

  /** Returns true if this is the special "all call" Device ID. */
  constexpr bool isAll() const { return (value_ == kAll); }

  /** Returns [0, 127] integer value (127 represents "all"). */
  constexpr std::uint8_t value() const { return value_; }

  // Equality operations:
  friend constexpr bool operator==(Device lhs, Device rhs) { return lhs.value_ == rhs.value_; }
  friend constexpr bool operator!=(Device lhs, Device rhs) { return lhs.value_ != rhs.value_; }

private:
  static constexpr std::uint8_t kAll = 0x7F;

  explicit constexpr Device(std::uint8_t value) : value_{value} {
    assert(value <= 0x7F);
  }

  std::uint8_t value_;
};

// Forward class declarations for friend access:
class MfrSysExBuilder;
class UniversalSysExBuilder;

/**
 * Manufacturer-specific (or private non-commercial) System Exclusive (SysEx)
 * message stored as contiguous bytes, which can NOT be resized after
 * initialization. Create new instances with MfrSysExBuilder.
 *
 * Either owns storage to message bytes allocated on the heap or references
 * an existing span of bytes provided at initialization time (which must
 * remain in scope for longer than this object for valid access).
 *
 * First byte of managed message data is the System Exclusive status byte (0xF0)
 * and the last byte of managed data (inclusive) is an End of Exlusive EOX
 * (0xF7) terminating status byte.
 *
 * After construction, all header and footer (EOX) bytes are written per the
 * SysEx format and provided Manufacturer ID, and the client should fill in
 * rawPayloadBytes() with the appropriate manufacturer-specific data.
 */
class MfrSysEx {
public:
  MfrSysEx(const MfrSysEx&) = delete;
  MfrSysEx& operator=(const MfrSysEx&) = delete;

  MfrSysEx(MfrSysEx&&) = default;
  MfrSysEx& operator=(MfrSysEx&&) = delete;

  /** Returns the Manufacturer this SysEx message is for. */
  Manufacturer manufacturer() const;

  /**
   * Returns read-only pointer to span of raw payload bytes, after the 1 or 3
   * byte manufacturer ID (and not including the terminating EOX byte).
   *
   * See numPayloadBytes() for size.
   */
  const std::uint8_t* rawPayloadBytes() const;

  /**
   * Returns read-write pointer to span of raw payload bytes, after the 1 or 3
   * byte manufacturer ID (and not including the terminating EOX byte).
   *
   * See numPayloadBytes() for size.
   */
  std::uint8_t* rawPayloadBytes();

  /**
   * Returns the # of payload bytes accessible through rawPayloadBytes(),
   * starting after the 1 or 3 bytes manufacturer ID (and not including the
   * terminating EOX byte).
   */
  int numPayloadBytes() const;

  /**
   * Returns read-only pointer to all message bytes, starting from the first
   * SysEx status byte (0xF0) and spanning through the terminating EOX (0xF7)
   * byte (inclusive).
   *
   * See numMsgBytesIncludingEox() for size.
   */
  const std::uint8_t* rawMsgBytes() const { return msgBytes_; }

  /**
   * Returns read-write pointer to all message bytes, starting from the first
   * SysEx status byte (0xF0) and spanning through the terminating EOX (0xF7)
   * byte (inclusive).
   *
   * See numMsgBytesIncludingEox() for size.
   */
  std::uint8_t* rawMsgBytes() { return msgBytes_; }

  /**
   * Returns the total # of bytes managed by this MfrSysEx message, from the
   * first SysEx status byte (0xF0) through the terminating EOX (0xF7) byte
   * (inclusive).
   */
  int numMsgBytesIncludingEox() const { return numMsgBytes_; }

private:
  friend class MfrSysExBuilder;

  explicit MfrSysEx(Manufacturer manufacturer, int numPayloadBytes);
  explicit MfrSysEx(Manufacturer manufacturer, std::uint8_t* rawMsgBytes, int numMsgBytes);

  bool hasExtMfrId() const;
  int numHeaderBytes() const;

  int numMsgBytes_;
  std::unique_ptr<std::uint8_t[]> heapAllocatedBytes_;  // Only set if this instance owns storage.
  std::uint8_t* msgBytes_;  // Pointer to first SysEx status byte (0xF0), within
                            // heapAllocatedBytes_, or referencing caller-provided storage.
};

/** Builder for creating instances of MfrSysEx messages. */
class MfrSysExBuilder {
public:
  explicit MfrSysExBuilder(Manufacturer manufacturer)
      : manufacturer_{manufacturer} {}

  /**
   * Sets the # of payload bytes, starting after the 1 or 3 bytes manufacturer
   * ID (and not including the terminating EOX byte).
   */
  MfrSysExBuilder withNumPayloadBytes(int numPayloadBytes);

  /**
   * Returns the total # of bytes that will be managed by the built MfrSysEx
   * message, from the first SysEx status byte (0xF0) through the terminating EOX
   * (0xF7) byte (inclusive).
   *
   * Can only be called AFTER withNumPayloadBytes().
   *
   * This is provided so clients managing their own data buffers for MIDI
   * message bytes can reserve the appropriate amount of storage, for use with
   * buildAsRefToBytes().
   */
  int numMsgBytesIncludingEox() const;

  /**
   * Builds a new MfrSysEx message for the configured manufacturer and # of
   * payload bytes, which will allocate and own storage for the full SysEx
   * message on the heap.
   *
   * Can only be called AFTER withNumPayloadBytes().
   */
  MfrSysEx buildOnHeap() const;

  /**
   * Builds a new MfrSysEx message for the configured manufacturer and # of
   * payload bytes, which will reference the provided rawMsgBytes (which must
   * remain in scope for longer than the built MfrSysEx for access to be valid).
   *
   * The given numMsgBytes must be exactly equal to numMsgBytesIncludingEox().
   *
   * Can only be called AFTER withNumPayloadBytes().
   */
  MfrSysEx buildAsRefToBytes(std::uint8_t* rawMsgBytes, int numMsgBytes) const;

private:
  static constexpr int kUnset = -1;

  Manufacturer manufacturer_;
  int numPayloadBytes_ = kUnset;
};

/**
 * Non-Realtime or Realtime Universal System Exclusive (SysEx) message stored as
 * contiguous bytes, which can NOT be resized after initialization. Create new
 * instances with UniversalSysExBuilder.
 *
 * Either owns storage to message bytes allocated on the heap or references
 * an existing span of bytes provided at initialization time (which must
 * remain in scope for longer than this object for valid access).
 *
 * First byte of managed message data is the System Exclusive status byte (0xF0)
 * and the last byte of managed data (inclusive) is an End of Exlusive EOX
 * (0xF7) terminating status byte.
 *
 * After construction, all header and footer (EOX) bytes are written per the
 * SysEx format and provided UniversalType and Device, and the client should
 * fill in rawPayloadBytes() with the appropriate data for the specific message.
 */
class UniversalSysEx {
public:
  UniversalSysEx(const UniversalSysEx&) = delete;
  UniversalSysEx& operator=(const UniversalSysEx&) = delete;

  UniversalSysEx(UniversalSysEx&&) = default;
  UniversalSysEx& operator=(UniversalSysEx&&) = delete;

  /** Returns the category of universal message this is (non-realtime or realtime). */
  UniversalCategory category() const;

  /** Returns the type of universal message this is. */
  UniversalType universalType() const;

  /**
   * Returns the device that should respond to this message (or special "all"
   * value that indicates message is for all receiving devices).
   */
  Device device() const;

  /** Sets the device (or special "all" value) that should respond. */
  void setDevice(Device device);

  /**
   * Returns read-only pointer to span of raw payload bytes, after the SysEx ID,
   * device ID, and 1 or 2 sub-ID bytes (and not including the terminating EOX
   * byte).
   *
   * See numPayloadBytes() for size.
   */
  const std::uint8_t* rawPayloadBytes() const;

  /**
   * Returns read-write pointer to span of raw payload bytes, after the SysEx
   * ID, device ID, and 1 or 2 sub-ID bytes (and not including the terminating
   * EOX byte).
   *
   * See numPayloadBytes() for size.
   */
  std::uint8_t* rawPayloadBytes();

  /**
   * Returns the # of payload bytes accessible through rawPayloadBytes(),
   * starting after the SysEx ID, device ID, and 1 or 2 sub-ID bytes (and not
   * including the terminating EOX byte).
   */
  int numPayloadBytes() const;

  /**
   * Returns read-only pointer to all message bytes, starting from the first
   * SysEx status byte (0xF0) and spanning through the terminating EOX (0xF7)
   * byte (inclusive).
   *
   * See numMsgBytesIncludingEox() for size.
   */
  const std::uint8_t* rawMsgBytes() const { return msgBytes_; }

  /**
   * Returns read-write pointer to all message bytes, starting from the first
   * SysEx status byte (0xF0) and spanning through the terminating EOX (0xF7)
   * byte (inclusive).
   *
   * See numMsgBytesIncludingEox() for size.
   */
  std::uint8_t* rawMsgBytes() { return msgBytes_; }

  /**
   * Returns the total # of bytes managed by this UniversalSysEx message, from
   * the first SysEx status byte (0xF0) through the terminating EOX (0xF7) byte
   * (inclusive).
   */
  int numMsgBytesIncludingEox() const { return numMsgBytes_; }

private:
  friend class UniversalSysExBuilder;

  explicit UniversalSysEx(UniversalType type, Device device, int numPayloadBytes);
  explicit UniversalSysEx(
      UniversalType type, Device device, std::uint8_t* rawMsgBytes, int numMsgBytes);

  bool typeHasSubId2() const;
  int numHeaderBytes() const;

  int numMsgBytes_;
  std::unique_ptr<std::uint8_t[]> heapAllocatedBytes_;  // Only set if this instance owns storage.
  std::uint8_t* msgBytes_;  // Pointer to first SysEx status byte (0xF0), within
                            // heapAllocatedBytes_, or referencing caller-provided storage.
};

/** Builder for creating instances of UniversalSysEx messages. */
class UniversalSysExBuilder {
public:
  explicit UniversalSysExBuilder(UniversalType type, Device device)
      : type_{type}, device_{device} {}

  /**
   * Sets the # of payload bytes, starting after the SysEx ID, device ID, and 1
   * or 2 sub-ID bytes (and not including the terminating EOX byte).
   */
  UniversalSysExBuilder withNumPayloadBytes(int numPayloadBytes);

  /**
   * Returns the total # of bytes that will be managed by the built UniversalSysEx
   * message, from the first SysEx status byte (0xF0) through the terminating EOX
   * (0xF7) byte (inclusive).
   *
   * Can only be called AFTER withNumPayloadBytes().
   *
   * This is provided so clients managing their own data buffers for MIDI
   * message bytes can reserve the appropriate amount of storage, for use with
   * buildAsRefToBytes().
   */
  int numMsgBytesIncludingEox() const;

  /**
   * Builds a new UniversalSysEx message for the configured type, device, and #
   * of payload bytes, which will allocate and own storage for the full SysEx
   * message on the heap.
   *
   * Can only be called AFTER withNumPayloadBytes().
   */
  UniversalSysEx buildOnHeap() const;

  /**
   * Builds a new UniversalSysEx message for the configured type, device, and #
   * of payload bytes, which will reference the provided rawMsgBytes (which must
   * remain in scope for longer than the built UniversalSysEx for access to be
   * valid).
   *
   * The given numMsgBytes must be exactly equal to numMsgBytesIncludingEox().
   *
   * Can only be called AFTER withNumPayloadBytes().
   */
  UniversalSysEx buildAsRefToBytes(std::uint8_t* rawMsgBytes, int numMsgBytes) const;

private:
  static constexpr int kUnset = -1;

  UniversalType type_;
  Device device_;
  int numPayloadBytes_ = kUnset;
};

namespace internal {

static constexpr int kOneSysExHdrStatusByte = 1;
static constexpr int kOneSysExTerminatingEoxByte = 1;

static constexpr int kNumManufacturerIdShortBytes = 1;
static constexpr int kNumManufacturerIdExtBytes = 3;

static constexpr int kNumUniversalHdrBytesOneSubId = 3;
static constexpr int kNumUniversalHdrBytesTwoSubIds = 4;

bool typeHasSubId2(UniversalCategory category, std::uint8_t subId1);

}  // namespace internal
}  // namespace bmmidi

#endif  // BMMIDI_SYSEX_HPP
