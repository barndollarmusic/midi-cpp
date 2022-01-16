// SPDX-FileCopyrightText: 2022 Barndollar Music, Ltd.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef BMMIDI_CONTROL_HPP
#define BMMIDI_CONTROL_HPP

#include <cassert>
#include <cstdint>

#include "bmmidi/cpp_features.hpp"
#include "bmmidi/data_value.hpp"

namespace bmmidi {

/** The # of MIDI Control values. */
BMMIDI_INLINE_VAR static constexpr int kNumControls = 128;

/**
 * Type of MIDI Control Change message, including special Channel Mode messages
 * CC120 - CC127. This enumeration includes several aliases for the same
 * numerical values.
 */
enum class Control : std::uint8_t {
  //============================================================================
  // By CC #
  //============================================================================

  k000 = 0x00,
  k001 = 0x01,
  k002 = 0x02,
  k003 = 0x03,
  k004 = 0x04,
  k005 = 0x05,
  k006 = 0x06,
  k007 = 0x07,
  k008 = 0x08,
  k009 = 0x09,
  k010 = 0x0A,
  k011 = 0x0B,
  k012 = 0x0C,
  k013 = 0x0D,
  k014 = 0x0E,
  k015 = 0x0F,

  k016 = 0x10,
  k017 = 0x11,
  k018 = 0x12,
  k019 = 0x13,
  k020 = 0x14,
  k021 = 0x15,
  k022 = 0x16,
  k023 = 0x17,
  k024 = 0x18,
  k025 = 0x19,
  k026 = 0x1A,
  k027 = 0x1B,
  k028 = 0x1C,
  k029 = 0x1D,
  k030 = 0x1E,
  k031 = 0x1F,

  k032 = 0x20,
  k033 = 0x21,
  k034 = 0x22,
  k035 = 0x23,
  k036 = 0x24,
  k037 = 0x25,
  k038 = 0x26,
  k039 = 0x27,
  k040 = 0x28,
  k041 = 0x29,
  k042 = 0x2A,
  k043 = 0x2B,
  k044 = 0x2C,
  k045 = 0x2D,
  k046 = 0x2E,
  k047 = 0x2F,

  k048 = 0x30,
  k049 = 0x31,
  k050 = 0x32,
  k051 = 0x33,
  k052 = 0x34,
  k053 = 0x35,
  k054 = 0x36,
  k055 = 0x37,
  k056 = 0x38,
  k057 = 0x39,
  k058 = 0x3A,
  k059 = 0x3B,
  k060 = 0x3C,
  k061 = 0x3D,
  k062 = 0x3E,
  k063 = 0x3F,

  k064 = 0x40,
  k065 = 0x41,
  k066 = 0x42,
  k067 = 0x43,
  k068 = 0x44,
  k069 = 0x45,
  k070 = 0x46,
  k071 = 0x47,
  k072 = 0x48,
  k073 = 0x49,
  k074 = 0x4A,
  k075 = 0x4B,
  k076 = 0x4C,
  k077 = 0x4D,
  k078 = 0x4E,
  k079 = 0x4F,

  k080 = 0x50,
  k081 = 0x51,
  k082 = 0x52,
  k083 = 0x53,
  k084 = 0x54,
  k085 = 0x55,
  k086 = 0x56,
  k087 = 0x57,
  k088 = 0x58,
  k089 = 0x59,
  k090 = 0x5A,
  k091 = 0x5B,
  k092 = 0x5C,
  k093 = 0x5D,
  k094 = 0x5E,
  k095 = 0x5F,

  k096 = 0x60,
  k097 = 0x61,
  k098 = 0x62,
  k099 = 0x63,
  k100 = 0x64,
  k101 = 0x65,
  k102 = 0x66,
  k103 = 0x67,
  k104 = 0x68,
  k105 = 0x69,
  k106 = 0x6A,
  k107 = 0x6B,
  k108 = 0x6C,
  k109 = 0x6D,
  k110 = 0x6E,
  k111 = 0x6F,

  k112 = 0x70,
  k113 = 0x71,
  k114 = 0x72,
  k115 = 0x73,
  k116 = 0x74,
  k117 = 0x75,
  k118 = 0x76,
  k119 = 0x77,
  k120 = 0x78,
  k121 = 0x79,
  k122 = 0x7A,
  k123 = 0x7B,
  k124 = 0x7C,
  k125 = 0x7D,
  k126 = 0x7E,
  k127 = 0x7F,

  //============================================================================
  // Least Significant Byte (LSB) Aliases (LSB for higher-precision CC0 - CC31)
  //============================================================================

  kLsb000 = 0x20,
  kLsb001 = 0x21,
  kLsb002 = 0x22,
  kLsb003 = 0x23,
  kLsb004 = 0x24,
  kLsb005 = 0x25,
  kLsb006 = 0x26,
  kLsb007 = 0x27,
  kLsb008 = 0x28,
  kLsb009 = 0x29,
  kLsb010 = 0x2A,
  kLsb011 = 0x2B,
  kLsb012 = 0x2C,
  kLsb013 = 0x2D,
  kLsb014 = 0x2E,
  kLsb015 = 0x2F,

  kLsb016 = 0x30,
  kLsb017 = 0x31,
  kLsb018 = 0x32,
  kLsb019 = 0x33,
  kLsb020 = 0x34,
  kLsb021 = 0x35,
  kLsb022 = 0x36,
  kLsb023 = 0x37,
  kLsb024 = 0x38,
  kLsb025 = 0x39,
  kLsb026 = 0x3A,
  kLsb027 = 0x3B,
  kLsb028 = 0x3C,
  kLsb029 = 0x3D,
  kLsb030 = 0x3E,
  kLsb031 = 0x3F,

  //============================================================================
  // Named Aliases (for standard specified controllers)
  //============================================================================

  /** Selects a [0, 127] bank number for following Program Change messages. */
  kBankSelect = k000,

  /** Modulation Wheel. */
  kModWheel = k001,

  /** Breath Controller. */
  kBreath = k002,

  /** Foot Pedal. */
  kFoot = k004,

  /** Portamento Time. */
  kPortamentoTime = k005,

  /**
   * Data Entry Most Significant Byte (MSB, upper 7 bits), to set value for
   * Registered Parameter Number (RPN) or Non-Registered Parameter Number
   * (NRPN).
   */
  kDataEntry = k006,

  /** Volume control for entire MIDI channel (all notes). */
  kChannelVolume = k007,

  /**
   * Controls volume balance between 2 sound sources.
   *
   * 0 (0x00): full volume left/lower half
   * 64 (0x40): equal value balance
   * 127 (0x7F): full volume right/upper half
   */
  kBalance = k008,

  /**
   * Stereo panning.
   *
   * 1 (0x01): hard left (0 also pans hard left)
   * 64 (0x40): center
   * 127 (0x7F): hard right
   * 
   * Recommended implementation:
   * L Gain (dB) = 20 * log(cos((pi / 2) * max(0, value - 1) / 126))
   * R Gain (dB) = 20 * log(sin((pi / 2) * max(0, value - 1) / 126))
   */
  kPan = k010,

  /** Expression, a "volume accent" applied on top of base Channel Volume. */
  kExpression = k011,

  kEffect1 = k012,
  kEffect2 = k013,

  kGeneralPurpose1 = k016,
  kGeneralPurpose2 = k017,
  kGeneralPurpose3 = k018,
  kGeneralPurpose4 = k019,

  kLsbBankSelect = k032,
  kLsbModWheel = k033,
  kLsbBreath = k034,
  kLsbFoot = k036,
  kLsbPortamentoTime = k037,

  /**
   * Data Entry Least Significant Byte (LSB, lower 7 bits), to set value for
   * Registered Parameter Number (RPN) or Non-Registered Parameter Number
   * (NRPN).
   */
  kLsbDataEntry = k038,

  kLsbChannelVolume = k039,
  kLsbBalance = k040,
  kLsbPan = k042,
  kLsbExpression = k043,
  kLsbEffect1 = k044,
  kLsbEffect2 = k045,
  kLsbGeneralPurpose1 = k048,
  kLsbGeneralPurpose2 = k049,
  kLsbGeneralPurpose3 = k050,
  kLsbGeneralPurpose4 = k051,

  kSustainPedal = k064,
  kPortamentoSwitch = k065,
  kSostenutoPedal = k066,
  kSoftPedal = k067,

  /**
   * Legato Footswitch, to enable monophonic synthesizer "legato" (usually
   * avoids re-triggering envelopes or attack portion of the sound).
   */
  kLegatoSwitch = k068,

  /** An alternate way to "hold" (i.e. sustain) notes, if supported. */
  kHold2 = k069,

  /**
   * Sound Controller #1, which defaults to Sound Variation. This may be used to
   * produce a variation of the primary sound (e.g. an overblown sax or pizzicato
   * string articulation).
   */
  kSoundVariation = k070,
  kSoundController01 = k070,

  /**
   * Sound Controller #2, which defaults to Timbre/Harmonic Intensity; also
   * frequently used for filter resonance.
   */
  kTimbreOrResonance = k071,
  kSoundController02 = k071,

  /** Sound Controller #3, which defaults to Release Time. */
  kReleaseTime = k072,
  kSoundController03 = k072,

  /** Sound Controller #4, which defaults to Attack Time. */
  kAttackTime = k073,
  kSoundController04 = k073,

  /**
   * Sound Controller #5, which defaults to Brightness; frequently used for
   * filter cutoff frequency.
   */
  kBrightness = k074,
  kSoundController05 = k074,

  /** Sound Controller #6, which defaults to Decay Time. */
  kDecayTime = k075,
  kSoundController06 = k075,

  /** Sound Controller #7, which defaults to Vibrato Rate. */
  kVibratoRate = k076,
  kSoundController07 = k076,

  /** Sound Controller #8, which defaults to Vibrato Depth. */
  kVibratoDepth = k077,
  kSoundController08 = k077,

  /** Sound Controller #9, which defaults to Vibrato Delay. */
  kVibratoDelay = k078,
  kSoundController09 = k078,

  /** Sound Controller #10 (no default). */
  kSoundController10 = k079,

  kGeneralPurpose5 = k080,
  kGeneralPurpose6 = k081,
  kGeneralPurpose7 = k082,
  kGeneralPurpose8 = k083,

  /**
   * Portamento Control, with [0, 127] value setting the source MIDI key
   * number that the next received Note On (on the same channel) will glide
   * from.
   */
  kPortamentoControl = k084,

  /**
   * Least Significant Byte (LSB) for High-Resolution Velocity Prefix. Applies
   * to the next received Note On or Note Off message.
   *
   * Afterward, the LSB implicitly resets to 0.
   *
   * Note that the minimum Note On velocity MSB is still 1, so there are
   * 127 x 128 = 16,256 possible Note On velocity values.
   */
  kLsbVelocity = k088,

  /** Effects #1 Depth, which defaults to Reverb Send Level. */
  kReverbLevel = k091,
  kEffect1Depth = k091,

  /** Effects #2 Depth, which defaults to Tremolo. */
  kTremolo = k092,
  kEffect2Depth = k092,

  /** Effects #3 Depth, which defaults to Chorus Send Level. */
  kChorusLevel = k093,
  kEffect3Depth = k093,

  /** Effects #4 Depth, which defaults to "Celeste" Detune. */
  kDetune = k094,
  kEffect4Depth = k094,

  /** Effects #5 Depth, which defaults to Phaser. */
  kPhaser = k095,
  kEffect5Depth = k095,

  /**
   * Increments value for Registered Parameter Number (RPN) or Non-Registered
   * Parameter Number (NRPN).
   *
   * Sometimes the controller value is used as the increment amount, but
   * frequently receiving devices will simply increment the parameter by some
   * default step size.
   */
  kDataIncrement = k096,

  /**
   * Decrements value for Registered Parameter Number (RPN) or Non-Registered
   * Parameter Number (NRPN).
   *
   * Sometimes the controller value is used as the decrement amount, but
   * frequently receiving devices will simply decrement the parameter by some
   * default step size.
   */
  kDataDecrement = k097,

  /**
   * Least-Significant Byte (lower 7 bits) of Non-Registered Parameter Number
   * (NRPN) to set via a following Data Entry, Data Increment, or Data Decrement
   * Control Change.
   */
  kLsbNRPN = k098,

  /**
   * Most-Significant Byte (upper 7 bits) of Non-Registered Parameter Number
   * (NRPN) to set via a following Data Entry, Data Increment, or Data Decrement
   * Control Change.
   */
  kNRPN = k099,

  /**
   * Least-Significant Byte (lower 7 bits) of Registered Parameter Number (RPN)
   * to set via a following Data Entry, Data Increment, or Data Decrement
   * Control Change.
   */
  kLsbRPN = k100,

  /**
   * Most-Significant Byte (upper 7 bits) of Registered Parameter Number (RPN)
   * to set via a following Data Entry, Data Increment, or Data Decrement
   * Control Change.
   */
  kRPN = k101,

  /**
   * Channel Mode: All Sound Off. If used, this should be sent after All Notes
   * Off.
   * Value (unused) should be 0.
   */
  kAllSoundOff = k120,

  /**
   * Channel Mode: Reset All Controllers (to defaults for current program).
   * Value (unused) should be 0.
   */
  kResetAllControllers = k121,

  /**
   * Channel Mode: Local Control On/Off, affecting whether a MIDI device should
   * enable MIDI input from its local MIDI control (e.g. built-in piano
   * keyboard).
   *
   * Value 0 for Off; 127 for On.
   */
  kLocalControlSwitch = k122,

  /**
   * Channel Mode: All Notes Off. Requests that the receiving MIDI device
   * stop all currently sounding notes (except perhaps those that are being
   * played via the device's Local Control). Should be sent before All Sound
   * Off.
   * 
   * Because not all receivers implement support for this, it is recommended to
   * send 128 individual Note Off messages before this.
   * 
   * Value (unused) should be 0.
   */
  kAllNotesOff = k123,

  /**
   * Channel Mode: Disables Omni Mode.
   * Value (unused) should be 0.
   */
  kOmniModeOff = k124,

  /**
   * Channel Mode: Enables Omni Mode (the default).
   * Value (unused) should be 0.
   */
  kOmniModeOn = k125,

  /**
   * Channel Mode: Enables Mono Mode (which disables Poly Mode).
   *
   * Value is the # of voices and associated contiguous MIDI channels
   * (starting from the current channel as the "basic channel"), or 0
   * indicates the maximum # of voices/channels.
   */
  kMonoMode = k126,

  /**
   * Channel Mode: Enables Poly Mode (which disables Mono Mode).
   * Value (unused) should be 0.
   */
  kPolyMode = k127,
};

/** Returns true if value is in valid [0, 127] Control range. */
constexpr bool controlNumberIsInRange(int value) {
  return (0 <= value) && (value <= 127);
}

/** Returns Control with the given [0, 127] numerical value. */
constexpr Control controlFromNumber(int value) {
  assert(controlNumberIsInRange(value));
  return static_cast<Control>(value);
}

/** Returns [0, 127] numerical value of given Control. */
constexpr std::int8_t controlToNumber(Control control) {
  return static_cast<std::int8_t>(control);
}

/** Returns Control from [0, 127] DataValue. */
constexpr Control controlFromDataValue(DataValue dataValue) {
  return static_cast<Control>(dataValue.value());
}

/** Returns [0, 127] as a DataValue. */
constexpr DataValue controlToDataValue(Control control) {
  return DataValue(controlToNumber(control));
}

}  // namespace bmmidi

#endif  // BMMIDI_CONTROL_HPP
