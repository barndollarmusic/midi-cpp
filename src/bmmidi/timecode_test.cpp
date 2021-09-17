#include "bmmidi/timecode.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

using ::testing::Eq;
using ::testing::IsFalse;
using ::testing::IsTrue;

TEST(MtcFullFrame, InitsToZero) {
  const auto t0_24fps = bmmidi::MtcFullFrame::zero(bmmidi::MtcFrameRateStandard::k24NonDrop);
  EXPECT_THAT(t0_24fps.frameRate(), Eq(bmmidi::MtcFrameRateStandard::k24NonDrop));
  EXPECT_THAT(t0_24fps.hh(), Eq(0));
  EXPECT_THAT(t0_24fps.mm(), Eq(0));
  EXPECT_THAT(t0_24fps.ss(), Eq(0));
  EXPECT_THAT(t0_24fps.ff(), Eq(0));
  EXPECT_THAT(t0_24fps.isValid(), IsTrue());

  const auto t0_25fps = bmmidi::MtcFullFrame::zero(bmmidi::MtcFrameRateStandard::k25NonDrop);
  EXPECT_THAT(t0_25fps.frameRate(), Eq(bmmidi::MtcFrameRateStandard::k25NonDrop));
  EXPECT_THAT(t0_25fps.hh(), Eq(0));
  EXPECT_THAT(t0_25fps.mm(), Eq(0));
  EXPECT_THAT(t0_25fps.ss(), Eq(0));
  EXPECT_THAT(t0_25fps.ff(), Eq(0));
  EXPECT_THAT(t0_25fps.isValid(), IsTrue());

  const auto t0_ntsc = bmmidi::MtcFullFrame::zero(bmmidi::MtcFrameRateStandard::k29Dot97Drop);
  EXPECT_THAT(t0_ntsc.frameRate(), Eq(bmmidi::MtcFrameRateStandard::k29Dot97Drop));
  EXPECT_THAT(t0_ntsc.hh(), Eq(0));
  EXPECT_THAT(t0_ntsc.mm(), Eq(0));
  EXPECT_THAT(t0_ntsc.ss(), Eq(0));
  EXPECT_THAT(t0_ntsc.ff(), Eq(0));
  EXPECT_THAT(t0_ntsc.isValid(), IsTrue());

  const auto t0_30fps = bmmidi::MtcFullFrame::zero(bmmidi::MtcFrameRateStandard::k30NonDrop);
  EXPECT_THAT(t0_30fps.frameRate(), Eq(bmmidi::MtcFrameRateStandard::k30NonDrop));
  EXPECT_THAT(t0_30fps.hh(), Eq(0));
  EXPECT_THAT(t0_30fps.mm(), Eq(0));
  EXPECT_THAT(t0_30fps.ss(), Eq(0));
  EXPECT_THAT(t0_30fps.ff(), Eq(0));
  EXPECT_THAT(t0_30fps.isValid(), IsTrue());
}

TEST(MtcFullFrame, CanMutateToInvalidIntermediateValues) {
  // Intentionally allows intermediate invalid mutations, since a full timecode
  // value is often built up in pieces (as with MTC Quarter Frame messages), and
  // it only needs to be valid once fully assembled.
  auto tc = bmmidi::MtcFullFrame::zero(bmmidi::MtcFrameRateStandard::k25NonDrop);

  tc.setFF(28);  // Not valid at 25 fps (but would be ok at 29.97 or 30 fps, so allowed).
  EXPECT_THAT(tc.isValid(), IsFalse());

  tc.setFrameRate(bmmidi::MtcFrameRateStandard::k29Dot97Drop);
  EXPECT_THAT(tc.isValid(), IsTrue());

  // At 29.97 fps, 23:34:00:01 would be a dropped frame.
  tc.setHH(23);
  tc.setMM(34);
  tc.setSS(0);
  tc.setFF(1);
  EXPECT_THAT(tc.isValid(), IsFalse());

  // But 23:34:01:01 is just fine.
  tc.setSS(1);
  EXPECT_THAT(tc.isValid(), IsTrue());

  // Make sure accessors yield correct values:
  EXPECT_THAT(tc.frameRate(), Eq(bmmidi::MtcFrameRateStandard::k29Dot97Drop));
  EXPECT_THAT(tc.hh(), Eq(23));
  EXPECT_THAT(tc.mm(), Eq(34));
  EXPECT_THAT(tc.ss(), Eq(1));
  EXPECT_THAT(tc.ff(), Eq(1));
}

TEST(MtcFullFrame, CanSetFromQuarterFrames) {
  // Can initialize timecode with any value, and then build up piece-by-piece.
  auto tc = bmmidi::MtcFullFrame::zero(bmmidi::MtcFrameRateStandard::k24NonDrop);

  // Set timecode of:
  //   - Rate: 25.000 fps (rr       = 01      )
  //   - HH: 23           (  h'hhhh =   1'0111)
  //   - MM: 59           ( mm'mmmm =  11'1011)
  //   - SS: 07           ( ss'ssss =  00'0111)
  //   - FF: 24           (  f'ffff =   1'1000)

  // Quarter Frame Data Bytes (nnn=piece, dddd=value).
  //                                    0nnn'dddd
  tc.setPieceFromQuarterFrameDataByte(0b0000'1000);  // FF lower 4 bits.
  tc.setPieceFromQuarterFrameDataByte(0b0001'0001);  // FF upper 1 bit.
  tc.setPieceFromQuarterFrameDataByte(0b0010'0111);  // SS lower 4 bits.
  tc.setPieceFromQuarterFrameDataByte(0b0011'0000);  // SS upper 2 bits.
  tc.setPieceFromQuarterFrameDataByte(0b0100'1011);  // MM lower 4 bits.
  tc.setPieceFromQuarterFrameDataByte(0b0101'0011);  // MM upper 2 bits.
  tc.setPieceFromQuarterFrameDataByte(0b0110'0111);  // HH lower 4 bits.
  tc.setPieceFromQuarterFrameDataByte(0b0111'0011);  // Rate + HH (0rrh) upper 4 bits.

  EXPECT_THAT(tc.frameRate(), Eq(bmmidi::MtcFrameRateStandard::k25NonDrop));
  EXPECT_THAT(tc.hh(), Eq(23));
  EXPECT_THAT(tc.mm(), Eq(59));
  EXPECT_THAT(tc.ss(), Eq(7));
  EXPECT_THAT(tc.ff(), Eq(24));
  EXPECT_THAT(tc.isValid(), IsTrue());
}

TEST(MtcFullFrame, CanSetInvalidIntermediateValuesFromQuarterFrames) {
  auto tc = bmmidi::MtcFullFrame::zero(bmmidi::MtcFrameRateStandard::k25NonDrop);
  tc.setHH(23);  // Max valid hour is 23.

  // Validation doesn't happen during setPieceFromQuarterFrameDataByte(), since
  // an intermediate value may not be valid.

  // Changing hour from 23 (0b1'0111)
  //                  to 8 (0b0'1000)
  // with the lower 4 bits set first, will temporarily yield a value
  //                 of 24 (0b1'1000)

  // Quarter Frame Data Bytes (nnn=piece, dddd=value).
  //                                    0nnn'dddd
  tc.setPieceFromQuarterFrameDataByte(0b0110'1000);  // HH lower 4 bits.

  EXPECT_THAT(tc.hh(), Eq(24));
  EXPECT_THAT(tc.isValid(), IsFalse());

  // Can now set the upper bit to restore validity.
  tc.setPieceFromQuarterFrameDataByte(0b0111'0010);  // Rate + HH (0rrh) upper 4 bits.
  
  EXPECT_THAT(tc.hh(), Eq(8));
  EXPECT_THAT(tc.isValid(), IsTrue());
}

TEST(MtcFullFrame, CanReadAsQuarterFrames) {
  // Set timecode of:
  //   - Rate: 25.000 fps (rr       = 01      )
  //   - HH: 23           (  h'hhhh =   1'0111)
  //   - MM: 59           ( mm'mmmm =  11'1011)
  //   - SS: 07           ( ss'ssss =  00'0111)
  //   - FF: 24           (  f'ffff =   1'1000)
  auto tc = bmmidi::MtcFullFrame::zero(bmmidi::MtcFrameRateStandard::k25NonDrop);
  tc.setHH(23);
  tc.setMM(59);
  tc.setSS(7);
  tc.setFF(24);

  EXPECT_THAT(tc.quarterFrameDataByteFor(bmmidi::MtcQuarterFramePiece::k0FrameLowerBits),
              Eq(0b0000'1000));
  EXPECT_THAT(tc.quarterFrameDataByteFor(bmmidi::MtcQuarterFramePiece::k1FrameUpperBits),
              Eq(0b0001'0001));
  EXPECT_THAT(tc.quarterFrameDataByteFor(bmmidi::MtcQuarterFramePiece::k2SecLowerBits),
              Eq(0b0010'0111));
  EXPECT_THAT(tc.quarterFrameDataByteFor(bmmidi::MtcQuarterFramePiece::k3SecUpperBits),
              Eq(0b0011'0000));
  EXPECT_THAT(tc.quarterFrameDataByteFor(bmmidi::MtcQuarterFramePiece::k4MinLowerBits),
              Eq(0b0100'1011));
  EXPECT_THAT(tc.quarterFrameDataByteFor(bmmidi::MtcQuarterFramePiece::k5MinUpperBits),
              Eq(0b0101'0011));
  EXPECT_THAT(tc.quarterFrameDataByteFor(bmmidi::MtcQuarterFramePiece::k6RateHourLowerBits),
              Eq(0b0110'0111));
  EXPECT_THAT(tc.quarterFrameDataByteFor(bmmidi::MtcQuarterFramePiece::k7RateHourUpperBits),
              Eq(0b0111'0011));
}

}  // namespace
