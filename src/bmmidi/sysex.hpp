#ifndef BMMIDI_SYSEX_HPP
#define BMMIDI_SYSEX_HPP

#include <cassert>
#include <cstdint>

namespace bmmidi {

/**
 * Specific type of System Exclusive (SysEx) message, based on the first data
 * byte (the System Exclusive Sub-ID).
 */
enum class SysExMessageType : std::uint8_t {
  /** Specific to a manufacturer, identified by a registered manufacturer ID. */
  kManufacturerSpecific = 0x00,

  /** Reserved for non-public uses by schools, research projects, etc. */
  kNonCommercial = 0x7D,

  /** Standardized Universal System Exclusive Extensions (non-real time). */
  kUniversalNonRealTime = 0x7E,

  /** Standardized Universal System Exclusive Extensions (real time). */
  kUniversalRealTime = 0x7F,
};

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
    assert((0x01 <= shortMfrId)
        && (shortMfrId < static_cast<std::uint8_t>(SysExMessageType::kNonCommercial)));
    return Manufacturer{shortMfrId, 0x00, 0x00};
  }

  /**
   * Returns Manufacturer with given extended 2-byte ID (not including the leading 0x00
   * SysEx Sub-ID).
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
    return Manufacturer{static_cast<std::uint8_t>(SysExMessageType::kNonCommercial), 0x00, 0x00};
  }

  /** Returns true if this is an extended 2-byte manufacturer ID. */
  constexpr bool isExtended() const { return (subId() == 0x00); }

  /**
   * Returns SysEx Sub-ID, which will be the short 1-byte manufacturer ID or
   * 0x00 if this is an extended manufacturer ID.
   */
  constexpr std::uint8_t subId() const { return bytes_[0]; }

  /**
   * Returns 1st byte of extended manufacturer ID (not including leading 0x00 of
   * the SysEx Sub-ID). Only valid if isExtended().
   */
  constexpr std::uint8_t extByte1() const {
    assert(isExtended());
    return bytes_[1];
  }

  /**
   * Returns 1st byte of extended manufacturer ID (not including leading 0x00 of
   * the SysEx Sub-ID). Only valid if isExtended().
   */
  constexpr std::uint8_t extByte2() const {
    assert(isExtended());
    return bytes_[2];
  }

  // Equality operations:
  friend constexpr bool operator==(Manufacturer lhs, Manufacturer rhs) {
    return (lhs.bytes_[0] == rhs.bytes_[0])
        && (lhs.bytes_[1] == rhs.bytes_[1])
        && (lhs.bytes_[2] == rhs.bytes_[2]);
  }

  friend constexpr bool operator!=(Manufacturer lhs, Manufacturer rhs) { return !(lhs == rhs); }

private:
  constexpr explicit Manufacturer(
      std::uint8_t subId, std::uint8_t extByte1, std::uint8_t extByte2)
      : bytes_{subId, extByte1, extByte2} {}

  // [0] SysEx Sub-ID (0x00 if extended, 0x01-0x7D otherwise)
  // [1] Extended Manufacturer ID byte 1 (or 0x00)
  // [2] Extended Manufacturer ID byte 2 (or 0x00)
  std::uint8_t bytes_[3];
};

}  // namespace bmmidi

#endif  // BMMIDI_SYSEX_HPP
