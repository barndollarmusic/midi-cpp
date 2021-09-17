#include "bmmidi/timecode.hpp"

#include <cassert>

#include "bmmidi/bitops.hpp"

namespace bmmidi {
namespace {

constexpr std::uint8_t kMtcFullFrameRateBits = 0b0110'0000;
constexpr std::uint8_t kMtcFullFrameHHBits = 0b0001'1111;
constexpr std::uint8_t kMtcFullFrameMMBits = 0b0011'1111;
constexpr std::uint8_t kMtcFullFrameSSBits = 0b0011'1111;
constexpr std::uint8_t kMtcFullFrameFFBits = 0b0001'1111;

constexpr int kMaxValidHour = 23;
constexpr int kMaxValidMin = 59;
constexpr int kMaxValidSec = 59;

bool isPieceForUpperBits(MtcQuarterFramePiece piece) {
  return (piece == MtcQuarterFramePiece::k1FrameUpperBits)
      || (piece == MtcQuarterFramePiece::k3SecUpperBits)
      || (piece == MtcQuarterFramePiece::k5MinUpperBits)
      || (piece == MtcQuarterFramePiece::k7RateHourUpperBits);
}

struct MtcFullByteInfo {
  int index;
  std::uint8_t bitMask;
};

MtcFullByteInfo mtcFullByteInfoFor(MtcQuarterFramePiece piece) {
  switch (piece) {
    case MtcQuarterFramePiece::k0FrameLowerBits:
      return {3, 0b0000'1111};

    case MtcQuarterFramePiece::k1FrameUpperBits:
      return {3, 0b0001'0000};

    case MtcQuarterFramePiece::k2SecLowerBits:
      return {2, 0b0000'1111};

    case MtcQuarterFramePiece::k3SecUpperBits:
      return {2, 0b0011'0000};

    case MtcQuarterFramePiece::k4MinLowerBits:
      return {1, 0b0000'1111};

    case MtcQuarterFramePiece::k5MinUpperBits:
      return {1, 0b0011'0000};

    case MtcQuarterFramePiece::k6RateHourLowerBits:
      return {0, 0b0000'1111};

    case MtcQuarterFramePiece::k7RateHourUpperBits:
      return {0, 0b0111'0000};

    default:
      assert(false);  // Invalid piece value.
      return {0, 0b0000'0000};
  }
}

int maxFrameNumberFor(MtcFrameRateStandard frameRate) {
  switch (frameRate) {
    case MtcFrameRateStandard::k24NonDrop:
      return 23;

    case MtcFrameRateStandard::k25NonDrop:
      return 24;

    case MtcFrameRateStandard::k29Dot97Drop:
    case MtcFrameRateStandard::k30NonDrop:
      return 29;

    default:
      assert(false);  // Invalid frame rate value.
      return 0;
  }
}

bool isDropFrame(MtcFrameRateStandard frameRate, int mm, int ss, int ff) {
  if (frameRate != MtcFrameRateStandard::k29Dot97Drop) {
    return false;  // Not a drop frame standard.
  }

  // In 29.97, the first 2 frames of minutes x1, x2, ..., x9 are dropped.
  return (ss == 0) && ((mm % 10) != 0) && ((ff == 0) || (ff == 1));
}

}  // namespace

MtcFullFrame MtcFullFrame::zero(MtcFrameRateStandard frameRate) {
  return MtcFullFrame{static_cast<std::uint8_t>(frameRate), 0x00, 0x00, 0x00};
}

MtcFrameRateStandard MtcFullFrame::frameRate() const {
  return static_cast<MtcFrameRateStandard>(
      internal::getBits(bytes_[0], kMtcFullFrameRateBits));
}

void MtcFullFrame::setFrameRate(MtcFrameRateStandard frameRate) {
  bytes_[0] = internal::setBits(bytes_[0], static_cast<std::uint8_t>(frameRate),
                                kMtcFullFrameRateBits);
}

int MtcFullFrame::hh() const {
  return internal::getBits(bytes_[0], kMtcFullFrameHHBits);
}

void MtcFullFrame::setHH(int hour) {
  assert(0 <= hour);
  assert(hour <= kMaxValidHour);

  bytes_[0] = internal::setBits(bytes_[0], static_cast<std::uint8_t>(hour),
                                kMtcFullFrameHHBits);
}

int MtcFullFrame::mm() const {
  return internal::getBits(bytes_[1], kMtcFullFrameMMBits);
}

void MtcFullFrame::setMM(int min) {
  assert(0 <= min);
  assert(min <= kMaxValidMin);

  bytes_[1] = internal::setBits(bytes_[1], static_cast<std::uint8_t>(min),
                                kMtcFullFrameMMBits);
}

int MtcFullFrame::ss() const {
  return internal::getBits(bytes_[2], kMtcFullFrameSSBits);
}

void MtcFullFrame::setSS(int sec) {
  assert(0 <= sec);
  assert(sec <= kMaxValidSec);

  bytes_[2] = internal::setBits(bytes_[2], static_cast<std::uint8_t>(sec),
                                kMtcFullFrameSSBits);
}

int MtcFullFrame::ff() const {
  return internal::getBits(bytes_[3], kMtcFullFrameFFBits);
}

void MtcFullFrame::setFF(int frame) {
  assert(0 <= frame);
  assert(frame <= 29);  // Only check max across all frame rate standards.

  bytes_[3] = internal::setBits(bytes_[3], static_cast<std::uint8_t>(frame),
                                kMtcFullFrameFFBits);
}

bool MtcFullFrame::isValid() const {
  const auto rate = frameRate();
  const auto hour = hh();
  const auto min = mm();
  const auto sec = ss();
  const auto frame = ff();

  // NOTE: Because of bitmasking, negative components shouldn't be possible.
  assert((0 <= hour) && (0 <= min) && (0 <= sec) && (0 <= frame));
  return (hour <= kMaxValidHour)
      && (min <= kMaxValidMin)
      && (sec <= kMaxValidSec)
      && (frame <= maxFrameNumberFor(rate))
      && !isDropFrame(rate, min, sec, frame);
}

std::uint8_t MtcFullFrame::quarterFrameDataByteFor(MtcQuarterFramePiece piece) const {
  const auto byteInfo = mtcFullByteInfoFor(piece);

  // Value is always stored in lower 4 bits of quarter frame data byte. If this
  // piece represents the upper bits, those need to be shifted lower.
  auto value = internal::getBits(bytes_[byteInfo.index], byteInfo.bitMask);
  value >>= isPieceForUpperBits(piece) ? 4 : 0;

  // Which piece this is then gets encoded in the upper bits.
  return static_cast<std::uint8_t>(piece) | value;
}

void MtcFullFrame::setPieceFromQuarterFrameDataByte(std::uint8_t quarterFrameDataByte) {
  const auto piece = static_cast<MtcQuarterFramePiece>(
      internal::getBits(quarterFrameDataByte, internal::kMtcQuarterFramePieceBits));

  // Value is always stored in lower 4 bits of quarter frame data byte.
  // If this piece represents the upper bits, those need to be shifted higher.
  auto value =
      internal::getBits(quarterFrameDataByte, internal::kMtcQuarterFrameValueBits);
  value <<= isPieceForUpperBits(piece) ? 4 : 0;

  const auto byteInfo = mtcFullByteInfoFor(piece);
  bytes_[byteInfo.index] = internal::setBits(bytes_[byteInfo.index], value, byteInfo.bitMask);
}

}  // namespace bmmidi
