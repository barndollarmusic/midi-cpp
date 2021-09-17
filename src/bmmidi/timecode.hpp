#ifndef BMMIDI_TIMECODE_HPP
#define BMMIDI_TIMECODE_HPP

#include <cstdint>

#include "bmmidi/cpp_features.hpp"

namespace bmmidi {
namespace internal {

BMMIDI_INLINE_VAR constexpr std::uint8_t kMtcQuarterFramePieceBits = 0b0111'0000;
BMMIDI_INLINE_VAR constexpr std::uint8_t kMtcQuarterFrameValueBits = 0b0000'1111;

}  // namespace internal

/** MIDI Time Code (MTC) supported frame rate standards. */
enum class MtcFrameRateStandard : std::uint8_t {
  /** 24.000 frames per second, non-drop. */
  k24NonDrop = 0x00,

  /** 25.000 frames per second, non-drop (PAL). */
  k25NonDrop = 0x20,

  /** 29.970 frames per second (strictly: 30,000 frames per 1,001 seconds), drop (NTSC). */
  k29Dot97Drop = 0x40,

  /** 30.000 frames per second, non-drop. */
  k30NonDrop = 0x60,
};

/**
 * MIDI Time Code (MTC) as used for ongoing synchronization is sent in 8 pieces,
 * over 8 quarter frames (so one timecode value is transmitted every two
 * frames).
 *
 * The first piece transmitted is for k0FrameLowerBits, which marks the moment
 * that the following transmitted timecode value takes place (the coded instant);
 * pieces are then sent in order, with piece k7RateHourUpperBits last.
 *
 * Whenever there is a jump in timecode, a Realtime Universal SysEx message
 * (of type bmmidi::universal::kMtcFull) is sent with the full MTC timecode
 * value.
 */
enum class MtcQuarterFramePiece : std::uint8_t {
  /** 4-byte value ffff is lower 4 bits of 5-bit frame index. */
  k0FrameLowerBits = 0x00,

  /** 4-byte value 000f is upper 1 bit of 5-bit frame index. */
  k1FrameUpperBits = 0x10,

  /** 4-byte value ssss is lower 4 bits of 6-bit second value. */
  k2SecLowerBits = 0x20,

  /** 4-byte value 00ss is upper 2 bits of 6-bit second value. */
  k3SecUpperBits = 0x30,

  /** 4-byte value mmmm is lower 4 bits of 6-bit minute value. */
  k4MinLowerBits = 0x40,

  /** 4-byte value 00mm is upper 2 bits of 6-bit minute value. */
  k5MinUpperBits = 0x50,

  /** 4-byte value hhhh is lower 4 bits of 5-bit hour value. */
  k6RateHourLowerBits = 0x60,

  /**
   * 4-byte value 0rrh is upper 1 bit of 5-bit hour value and the 2-bit frame
   * rate code.
   */
  k7RateHourUpperBits = 0x70,
};

/**
 * A full frame of MTC timecode, encoding the MtcFrameRateStandard and
 * HH:MM:SS:FF values. This may be transmitted in 8 quarter frames with
 * regular MTC Quarter Frame messages or all at once with a Realtime
 * Universal SysEx message (of type bmmidi::universal::kMtcFull).
 */
class MtcFullFrame {
public:
  /** Returns time 00:00:00:00 for the given MtcFrameRateStandard. */
  static MtcFullFrame zero(MtcFrameRateStandard frameRate);

  /** Returns the frame rate standard. */
  MtcFrameRateStandard frameRate() const;

  /**
   * Updates the frame rate standard.
   *
   * NOTE: This may cause the encoded HH:MM:SS:FF value to be invalid in the
   * new standard. See isValid() if you need to check for validity before using
   * this MtcFullFrame value.
   */
  void setFrameRate(MtcFrameRateStandard frameRate);

  /** Returns the 0-23 hour value. */
  int hh() const;

  /** Sets the 0-23 hour value. */
  void setHH(int hour);

  /** Returns the 0-59 minute value. */
  int mm() const;

  /**
   * Sets the 0-59 minute value.
   *
   * NOTE: In the 29.97 drop frame rate standard, changing the minute can
   * cause this MtcFullFrame to refer to an invalid dropped frame number.
   * See isValid() if you need to check for validity before using this
   * MtcFullFrame value.
   */
  void setMM(int min);

  /** Returns the 0-59 second value. */
  int ss() const;

  /**
   * Sets the 0-59 second value.
   *
   * NOTE: In the 29.97 drop frame rate standard, changing the second can
   * cause this MtcFullFrame to refer to an invalid dropped frame number.
   * See isValid() if you need to check for validity before using this
   * MtcFullFrame value.
   */
  void setSS(int sec);

  /** Returns the frame number (maximum depends on frame rate standard). */
  int ff() const;

  /**
   * Sets the frame number (maximum depends on frame rate standard).
   *
   * NOTE: This allows setting invalid frame numbers (which might refer to a
   * dropped frame or an out-of-range frame), so that MM or the frame rate
   * standard can be updated subsequently without triggering any intermediate
   * errors. See isValid() if you need to check for validity before using this
   * MtcFullFrame value.
   */
  void setFF(int frame);

  /**
   * Returns true if this represents a valid timecode (all HH, MM, SS, and FF
   * values in range for the configured frame rate standard and not representing
   * an invalid dropped frame number).
   */
  bool isValid() const;

  /**
   * Returns the encoded data byte representing the given quarter frame piece of
   * this full timecode value, for use with an MTC quarter frame message.
   */
  std::uint8_t quarterFrameDataByteFor(MtcQuarterFramePiece piece) const;

  /**
  * Updates one quarter frame piece of this timecode frame value based on the
  * given encoded MTC quarter frame message data byte.
  *
  * NOTE: This may allow setting invalid data, since validation only makes sense
  * once all pieces have been set. See isValid() if you need to check for
  * validity before using this MtcFullFrame value.
  */
  void setPieceFromQuarterFrameDataByte(std::uint8_t quarterFrameDataByte);

private:
  explicit MtcFullFrame(
      std::uint8_t rateHrByte, std::uint8_t minByte, std::uint8_t secByte, std::uint8_t frameByte)
      : bytes_{rateHrByte, minByte, secByte, frameByte} {}

  // bytes_[0]: 0b0rrh'hhhh (encodes framerate standard + hour).
  // bytes_[1]: 0b00mm'mmmm (encodes minute).
  // bytes_[2]: 0b00ss'ssss (encodes second).
  // bytes_[3]: 0b000f'ffff (encodes frame).
  std::uint8_t bytes_[4];
};

static_assert(sizeof(MtcFullFrame) == 4, "MtcFullFrame must be 4 bytes (no padding)");

// TODO: Add utilities for building a bmmidi::universal::kMtcFull SysEx message.

}  // namespace bmmidi

#endif  // BMMIDI_TIMECODE_HPP
