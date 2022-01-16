// SPDX-FileCopyrightText: 2022 Barndollar Music, Ltd.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef BMMIDI_STATUS_HPP
#define BMMIDI_STATUS_HPP

#include <cassert>
#include <cstdint>

#include "bmmidi/channel.hpp"

namespace bmmidi {

/** Type of a MIDI message, as determined by its status byte. */
enum class MsgType : std::uint8_t {
  //============================================================================
  // Channel Voice Messages (lower 4 bits select MIDI channel)
  //============================================================================

  /**
   * Note Off (key released).
   * 
   * 2 data bytes:
   * [1] Key Number
   * [2] Note Off Velocity (usually 0, but may affect release if supported)
   */
  kNoteOff = 0x80,

  /**
   * Note On (key pressed).
   *
   * 2 data bytes:
   * [1] Key Number
   * [2] Note On Velocity
   *
   * A note on message with a velocity value of 0 should be interpreted exactly
   * the same way as a Note Off.
   */
  kNoteOn = 0x90,

  /**
   * Polyphonic key pressure (per-key aftertouch).
   * 
   * 2 data bytes:
   * [1] Key Number
   * [2] Pressure Value
   */
  kPolyphonicKeyPressure = 0xA0,

  /**
   * Control Change (CC), including Channel Mode messages (CC120 - CC127).
   *
   * 2 data bytes:
   * [1] Control Number
   * [2] Control Value
   */
  kControlChange = 0xB0,

  /**
   * Program Change, i.e. to select a different patch or preset.
   * 
   * 1 data byte:
   * [1] Program Number
   */
  kProgramChange = 0xC0,

  /**
   * Channel Presssure (non-polyphonic aftertouch that applies to all keys).
   *
   * 1 data byte:
   * [1] Pressure Value
   */
  kChannelPressure = 0xD0,

  /**
   * Channel Pitch Bend Change (applies to all keys).
   *
   * Pitch Bend values are 14-bit unsigned integers in the [0, 16383] range,
   * where 0 represents minimum downward bend, 8192 represents centered position
   * (no pitch bend), and 16383 represents maximum upward bend.
   *
   * Note that 1 is often interpreted the same as 0 as the minimum downward
   * bend, for symmetry of upward and downward bend ranges.
   * 
   * 2 data bytes:
   * [1] Pitch Bend LSB (lower 7 bits)
   * [2] Pitch Bend MSB (upper 7 bits)
   */
  kPitchBend = 0xE0,

  //============================================================================
  // System Messages
  //============================================================================

  /** System Exclusive (SysEx) message, with a variable # of data bytes. */
  kSystemExclusive = 0xF0,

  /**
   * System Common: MIDI Time Code (MTC) Quarter Frame.
   *
   * 1 byte data value (lower 4 bits are value and upper 3 bits are MTC message
   * type, describing which piece of the timecode is being sent).
   */
  kMtcQuarterFrame = 0xF1,

  /**
   * System Common: Song Position Pointer.
   * 2 byte data value (14-bit integer), in units of 6 "MIDI clocks", which
   * correspond to a 1/16 note (regardless of a sequencer's internal ppq).
   */
  kSongPositionPointer = 0xF2,

  /**
   * System Common: Song Select.
   * 1 byte data value (for song number).
   */
  kSongSelect = 0xF3,

  /**
   * System Common: (analog oscillator) Tune Request.
   * 0 data bytes.
   */
  kOscillatorTuneRequest = 0xF6,

  /**
   * System Common: indicates end of a SysEx message.
   * 0 data bytes.
   */
  kEndOfSystemExclusive = 0xF7,

  /**
   * System Real Time: Timing Clock.
   * 0 data bytes.
   */
  kTimingClock = 0xF8,

  /**
   * System Real Time: Start Playback.
   * 0 data bytes.
   */
  kStart = 0xFA,

  /**
   * System Real Time: Continue Playback.
   * 0 data bytes.
   */
  kContinue = 0xFB,

  /**
   * System Real Time: Stop Playback.
   * 0 data bytes.
   */
  kStop = 0xFC,

  /**
   * System Real Time: Active Sensing.
   * 
   * Can be used to help detect a disconnected MIDI input device (e.g. if no
   * MIDI messages have been received after ~330 ms). Should only assume
   * disconnected if a device transmits these Active Sensing messages, however,
   * since this is an optional feature.
   *
   * 0 data bytes.
   */
  kActiveSensing = 0xFE,

  /**
   * System Real Time: System Reset.
   * 
   * Receiving device should set Omni On, Poly Mode, Local Control On, Voices
   * Off, reset all controllers, set song position to 0, stop playback, clear
   * running status, and reset instrument to its power-up condition.
   *
   * 0 data bytes.
   */
  kSystemReset = 0xFF,
};

/**
 * MIDI status byte, which indicates the type of message and (if applicable)
 * the MIDI channel.
 */
class Status {
public:
  /**
   * Special value used to indicate that a message has a variable # of data bytes.
   *
   * This currently only applies to SysEx messages.
   */
  static constexpr int kVariableDataBytes = -1;

  /**
   * Creates Status byte value for a Channel Voice message.
   * 
   * Input channel must be a normal [0, 15] index MIDI channel (since special
   * Channel 'none' and 'omni' values aren't supported as MIDI status
   * bytes).
   */
  static constexpr Status channelVoice(MsgType type, Channel channel) {
    assert(channel.isNormal());

    const std::uint8_t index = static_cast<std::uint8_t>(channel.index());
    const std::uint8_t baseType = static_cast<std::uint8_t>(type);
    const std::uint8_t value = baseType | index;
    const Status result{value};
    assert(result.isChannelSpecific());
    return result;
  }

  /** Creates Status byte for a System message. */
  static constexpr Status system(MsgType type) {
    const std::uint8_t baseType = static_cast<std::uint8_t>(type);
    const Status result{baseType};
    assert(!result.isChannelSpecific());
    return result;
  }

  /**
   * Creates Status byte from the input value, which must be in valid
   * [0x80, 0xFF] range.
   */
  explicit constexpr Status(std::uint8_t value) : value_{value} {
    assert(static_cast<std::uint8_t>(MsgType::kNoteOff) <= value_);
  }

  /**
   * Returns type of MIDI message this status represents, which is independent
   * of any particular MIDI channel.
   */
  constexpr MsgType type() const {
    // Clear channel when applicable to match MsgType value.
    return static_cast<MsgType>(isChannelSpecific() ? value_ & 0xF0 : value_);
  }

  /** Returns status byte value, in [0x80, 0xFF] range. */
  constexpr std::uint8_t value() const { return value_; }

  /**
   * Returns the # of data bytes that should follow a message of this type, or
   * kVariableDataBytes if there is not a fixed number (which currently only applies
   * to SysEx messages).
   */
  constexpr int numDataBytes() const {
    switch (type()) {
      case MsgType::kNoteOff:
      case MsgType::kNoteOn:
      case MsgType::kPolyphonicKeyPressure:
      case MsgType::kControlChange:
      case MsgType::kPitchBend:
      case MsgType::kSongPositionPointer:
        return 2;
      
      case MsgType::kProgramChange:
      case MsgType::kChannelPressure:
      case MsgType::kMtcQuarterFrame:
      case MsgType::kSongSelect:
        return 1;
      
      case MsgType::kSystemExclusive:
        return kVariableDataBytes;
      
      default:
        return 0;
    }
  }

  /**
   * Returns true if this status byte represents a Channel Voice or Channel Mode
   * (which is just a specific range of Control Change) message, which is
   * addressed to a specific [0, 15] index MIDI channel.
   */
  constexpr bool isChannelSpecific() const {
    return (value_ < static_cast<std::uint8_t>(MsgType::kSystemExclusive));
  }

  /**
   * Returns the MIDI channel that this message is addressed to.
   *
   * Error (fails assertion) to call if !isChannelSpecific().
   */
  constexpr Channel channel() const {
    assert(isChannelSpecific());
    return Channel::index(value_ & 0x0F);
  }

private:
  std::uint8_t value_;
};

// Support equality comparisons but not ordering.
constexpr bool operator==(Status lhs, Status rhs) { return lhs.value() == rhs.value(); }
constexpr bool operator!=(Status lhs, Status rhs) { return lhs.value() != rhs.value(); }

}  // namespace bmmidi

#endif  // BMMIDI_STATUS_HPP
