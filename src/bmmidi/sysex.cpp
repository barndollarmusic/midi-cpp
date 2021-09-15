#include "bmmidi/sysex.hpp"

#include "bmmidi/status.hpp"

namespace bmmidi {
namespace {

static constexpr int kOneSysExHdrStatusByte = 1;
static constexpr int kOneSysExTerminatingEoxByte = 1;

static constexpr int kNumManufacturerIdShortBytes = 1;
static constexpr int kNumManufacturerIdExtBytes = 3;

static constexpr int kNumUniversalHdrBytesOneSubId = 3;
static constexpr int kNumUniversalHdrBytesTwoSubIds = 4;

int numBytesForId(Manufacturer mfr) {
  return mfr.isExtended() ? 3 : 1;
}

int numMsgBytesFor(Manufacturer mfr, int numPayloadBytes) {
  return kOneSysExHdrStatusByte
      + (mfr.isExtended() ? kNumManufacturerIdExtBytes : kNumManufacturerIdShortBytes)
      + numPayloadBytes
      + kOneSysExTerminatingEoxByte;
}

int numMsgBytesFor(UniversalType type, int numPayloadBytes) {
  return kOneSysExHdrStatusByte
      + (type.hasSubId2() ? kNumUniversalHdrBytesTwoSubIds : kNumUniversalHdrBytesOneSubId)
      + numPayloadBytes
      + kOneSysExTerminatingEoxByte;
}

void writeSysExStatusBytes(std::uint8_t* rawMsgBytes, int numMsgBytes) {
  rawMsgBytes[0] = static_cast<std::uint8_t>(MsgType::kSystemExclusive);
  rawMsgBytes[numMsgBytes - 1] = static_cast<std::uint8_t>(MsgType::kEndOfSystemExclusive);
}

void writeManufacturerId(Manufacturer mfr, std::uint8_t* rawMsgBytes) {
  rawMsgBytes[1] = mfr.sysExId();
  if (mfr.isExtended()) {
    rawMsgBytes[2] = mfr.extByte1();
    rawMsgBytes[3] = mfr.extByte2();
  }
}

void writeUniversalHeaderBytes(UniversalType type, Device device, std::uint8_t* rawMsgBytes) {
  rawMsgBytes[1] = static_cast<std::uint8_t>(type.category());
  rawMsgBytes[2] = device.value();
  rawMsgBytes[3] = type.subId1();
  if (type.hasSubId2()) {
    rawMsgBytes[4] = type.subId2();
  }
}

}  // namespace

MfrSysEx::MfrSysEx(Manufacturer manufacturer, int numPayloadBytes)
    : numMsgBytes_{numMsgBytesFor(manufacturer, numPayloadBytes)},
      heapAllocatedBytes_{std::make_unique<std::uint8_t[]>(numMsgBytes_)},
      msgBytes_{heapAllocatedBytes_.get()} {
  // (Input arguments already validated by MfrSysExBuilder).

  writeSysExStatusBytes(msgBytes_, numMsgBytes_);
  writeManufacturerId(manufacturer, msgBytes_);
}

MfrSysEx::MfrSysEx(Manufacturer manufacturer, std::uint8_t* rawMsgBytes, int numMsgBytes)
    : numMsgBytes_{numMsgBytes},
      heapAllocatedBytes_{},
      msgBytes_{rawMsgBytes} {
  // (Input arguments already validated by MfrSysExBuilder).

  writeSysExStatusBytes(msgBytes_, numMsgBytes_);
  writeManufacturerId(manufacturer, msgBytes_);
}

bool MfrSysEx::hasExtMfrId() const { return (msgBytes_[1] == Manufacturer::kExtendedSysExId); }

int MfrSysEx::numHeaderBytes() const {
  return kOneSysExHdrStatusByte
      + (hasExtMfrId() ? kNumManufacturerIdExtBytes : kNumManufacturerIdShortBytes);
}

Manufacturer MfrSysEx::manufacturer() const {
  if (hasExtMfrId()) {
    return Manufacturer::extId(msgBytes_[2], msgBytes_[3]);
  }

  const bool isNonCommercial = (msgBytes_[1] == Manufacturer::kNonCommercialSysExId);
  return isNonCommercial ? Manufacturer::nonCommercial() : Manufacturer::shortId(msgBytes_[1]);
}

const std::uint8_t* MfrSysEx::rawPayloadBytes() const {
  return &msgBytes_[numHeaderBytes()];
}

std::uint8_t* MfrSysEx::rawPayloadBytes() {
  return const_cast<std::uint8_t*>(static_cast<const MfrSysEx*>(this)->rawPayloadBytes());
}

int MfrSysEx::numPayloadBytes() const {
  return numMsgBytes_ - numHeaderBytes() - kOneSysExTerminatingEoxByte;
}

MfrSysExBuilder MfrSysExBuilder::withNumPayloadBytes(int numPayloadBytes) {
  assert(numPayloadBytes_ == kUnset);
  assert(numPayloadBytes >= 1);

  numPayloadBytes_ = numPayloadBytes;
  return *this;
}

int MfrSysExBuilder::numMsgBytesIncludingEox() const {
  assert(numPayloadBytes_ != kUnset);
  return numMsgBytesFor(manufacturer_, numPayloadBytes_);
}

MfrSysEx MfrSysExBuilder::buildOnHeap() const {
  assert(numPayloadBytes_ != kUnset);
  return MfrSysEx{manufacturer_, numPayloadBytes_};
}

MfrSysEx MfrSysExBuilder::buildAsRefToBytes(std::uint8_t* rawMsgBytes, int numMsgBytes) const {
  assert(rawMsgBytes != nullptr);
  assert(numMsgBytes == numMsgBytesIncludingEox());
  return MfrSysEx{manufacturer_, rawMsgBytes, numMsgBytes};
}

UniversalSysEx::UniversalSysEx(UniversalType type, Device device, int numPayloadBytes)
    : type_{type},
      numMsgBytes_{numMsgBytesFor(type, numPayloadBytes)},
      heapAllocatedBytes_{std::make_unique<std::uint8_t[]>(numMsgBytes_)},
      msgBytes_{heapAllocatedBytes_.get()} {
  // (Input arguments already validated by UniversalSysExBuilder).

  writeSysExStatusBytes(msgBytes_, numMsgBytes_);
  writeUniversalHeaderBytes(type, device, msgBytes_);
}

UniversalSysEx::UniversalSysEx(
    UniversalType type, Device device, std::uint8_t* rawMsgBytes, int numMsgBytes)
    : type_{type},
      numMsgBytes_{numMsgBytes},
      heapAllocatedBytes_{},
      msgBytes_{rawMsgBytes} {
  // (Input arguments already validated by UniversalSysExBuilder).

  writeSysExStatusBytes(msgBytes_, numMsgBytes_);
  writeUniversalHeaderBytes(type, device, msgBytes_);
}

int UniversalSysEx::numHeaderBytes() const {
  return kOneSysExHdrStatusByte
      + (type_.hasSubId2() ? kNumUniversalHdrBytesTwoSubIds : kNumUniversalHdrBytesOneSubId);
}

Device UniversalSysEx::device() const {
  return (msgBytes_[2] == Device::all().value())
      ? Device::all()
      : Device::id(msgBytes_[2]);
}

const std::uint8_t* UniversalSysEx::rawPayloadBytes() const {
  return &msgBytes_[numHeaderBytes()];
}

std::uint8_t* UniversalSysEx::rawPayloadBytes() {
  return const_cast<std::uint8_t*>(static_cast<const UniversalSysEx*>(this)->rawPayloadBytes());
}

int UniversalSysEx::numPayloadBytes() const {
  return numMsgBytes_ - numHeaderBytes() - kOneSysExTerminatingEoxByte;
}

UniversalSysExBuilder UniversalSysExBuilder::withNumPayloadBytes(int numPayloadBytes) {
  assert(numPayloadBytes_ == kUnset);
  assert(numPayloadBytes >= 1);

  numPayloadBytes_ = numPayloadBytes;
  return *this;
}

int UniversalSysExBuilder::numMsgBytesIncludingEox() const {
  assert(numPayloadBytes_ != kUnset);
  return numMsgBytesFor(type_, numPayloadBytes_);
}

UniversalSysEx UniversalSysExBuilder::buildOnHeap() const {
  assert(numPayloadBytes_ != kUnset);
  return UniversalSysEx{type_, device_, numPayloadBytes_};
}

UniversalSysEx UniversalSysExBuilder::buildAsRefToBytes(
    std::uint8_t* rawMsgBytes, int numMsgBytes) const {
  assert(rawMsgBytes != nullptr);
  assert(numMsgBytes == numMsgBytesIncludingEox());
  return UniversalSysEx{type_, device_, rawMsgBytes, numMsgBytes};
}

}  // namespace bmmidi
