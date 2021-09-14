#ifndef BMMIDI_SYSEX_HPP
#define BMMIDI_SYSEX_HPP

#include <cassert>
#include <cstdint>

#include "bmmidi/cpp_features.hpp"

namespace bmmidi {

/**
 * ID for a registered MIDI manufacturer, which is either a 1-byte short ID
 * or a 2-byte extended ID. Extended IDs are in a different ID space from
 * short IDs (so the value 0x01 as a short ID is distinct from the value
 * 0x00 0x01 as an extended ID).
 *
 * Also includes a special non-commercial value, for private use by schools,
 * research projects, etc.
 *
 * See https://www.midi.org/specifications-old/item/manufacturer-id-numbers.
 */
class Manufacturer {
public:
  /**
   * Returns Manufacturer with given short 1-byte ID. For example, Sequential
   * Circuits has the short ID 0x01.
   */
  static constexpr Manufacturer shortId(std::uint8_t shortMfrId) {
    assert((0x01 <= shortMfrId) && (shortMfrId < kNonCommercial));
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
    return Manufacturer{0x00, extByte1, extByte2};
  }

  /**
   * Returns generic non-commercial Manufacturer that can be used privately by
   * schools, research projects, etc.
   */
  static constexpr Manufacturer nonCommercial() {
    return Manufacturer{kNonCommercial, 0x00, 0x00};
  }

  /** Returns true if this is an extended 2-byte manufacturer ID. */
  constexpr bool isExtended() const { return (sysExId_ == 0x00); }

  /**
   * Returns SysEx ID, which will be the short 1-byte manufacturer ID or
   * 0x00 if this is an extended manufacturer ID.
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
  static constexpr std::uint8_t kNonCommercial = 0x7D;

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

// TODO: Add constants for rest of UniversalType non-realtime values.

//==============================================================================
// Realtime (0x7F)
//==============================================================================

/** Realtime MIDI Time Code (MTC) Full Message. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcFull = UniversalType::realTime(0x01, 0x01);

/** Realtime MIDI Time Code (MTC) User Bits. */
BMMIDI_INLINE_VAR constexpr UniversalType kMtcUserBits = UniversalType::realTime(0x01, 0x02);

// TODO: Add constants for rest of UniversalType realtime values.

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

}  // namespace bmmidi

#endif  // BMMIDI_SYSEX_HPP
