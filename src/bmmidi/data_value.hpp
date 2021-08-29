#ifndef BMMIDI_DATA_VALUE_HPP
#define BMMIDI_DATA_VALUE_HPP

#include <cassert>
#include <cmath>
#include <cstdint>
#include <type_traits>

namespace bmmidi {

/** Represents a standard [0, 127] range (7-bit) integer MIDI data value. */
class DataValue {
public:
  /** Returns minimum 0 value. */
  static constexpr DataValue min() { return DataValue{kMin}; }

  /** Returns midpoint 64 value (e.g. represents center pan position). */
  static constexpr DataValue midpoint() { return DataValue{kMidpoint}; }

  /** Returns maximum 127 value. */
  static constexpr DataValue max() { return DataValue{kMax}; }

  /**
   * Maps an input normalized [0.0, 1.0] floating-point value to its
   * nearest corresponding integer in [0, 127] range.
   */
  template<typename FloatT>
  static constexpr DataValue fromNormalized0To1(FloatT normalizedValue) {
    static_assert(std::is_floating_point<FloatT>::value,
        "DataValue::fromNormalized0To1() requires a floating-point type");

    const auto rawValue = std::round(normalizedValue * static_cast<FloatT>(kMax));
    assert(kMin <= rawValue);
    assert(rawValue <= kMax);
    return DataValue{static_cast<std::int8_t>(rawValue)};
  }

  /** Creates a DataValue from the input [0, 127] value. */
  explicit constexpr DataValue(std::int8_t value) : value_{value} {
    assert(kMin <= value);
  }

  /** Returns [0, 127] integer value. */
  constexpr std::int8_t value() const { return value_; }

  /** Returns normalized [0.0, 1.0] floating-point value. */
  template<typename FloatT>
  FloatT toNormalized0To1() const {
    static_assert(std::is_floating_point<FloatT>::value,
        "DataValue::toNormalized0To1() requires a floating-point type");

    return static_cast<FloatT>(value_) / static_cast<FloatT>(kMax);
  }

  // Comparison operations:
  friend constexpr bool operator==(DataValue lhs, DataValue rhs) { return lhs.value_ == rhs.value_; }
  friend constexpr bool operator!=(DataValue lhs, DataValue rhs) { return lhs.value_ != rhs.value_; }
  friend constexpr bool operator<(DataValue lhs, DataValue rhs) { return lhs.value_ < rhs.value_; }
  friend constexpr bool operator<=(DataValue lhs, DataValue rhs) { return lhs.value_ <= rhs.value_; }
  friend constexpr bool operator>(DataValue lhs, DataValue rhs) { return lhs.value_ > rhs.value_; }
  friend constexpr bool operator>=(DataValue lhs, DataValue rhs) { return lhs.value_ >= rhs.value_; }

private:
  static constexpr std::int8_t kMin = 0;
  static constexpr std::int8_t kMidpoint = 64;
  static constexpr std::int8_t kMax = 127;

  std::int8_t value_;
};

/** Represents an extended [0, 16383] range (14-bit) integer MIDI data value. */
class DoubleDataValue {
public:
  /** Returns minimum 0 value. */
  static constexpr DoubleDataValue min() { return DoubleDataValue{kMin}; }

  /** Returns midpoint 8192 value (e.g. represents center pitch bend position). */
  static constexpr DoubleDataValue midpoint() { return DoubleDataValue{kMidpoint}; }

  /** Returns maximum 16383 value. */
  static constexpr DoubleDataValue max() { return DoubleDataValue{kMax}; }

  /**
   * Creates a DoubleDataValue from the input lsb (Least Significant Byte,
   * i.e. lower 7 bits) and msb (Most Significant Byte, i.e. upper 7 bits).
   */
  static constexpr DoubleDataValue fromLsbMsb(std::uint8_t lsb, std::uint8_t msb) {
    const std::uint16_t lowerValue = static_cast<std::uint16_t>(lsb) & kLsbBitmask;
    const std::uint16_t upperValue =
        (static_cast<std::uint16_t>(msb) << kDataBitsPerByte) & kMsbBitmask;
    return DoubleDataValue{static_cast<std::int16_t>(upperValue | lowerValue)};
  }

  /** Creates a DoubleDataValue from the input [0, 16383] value. */
  explicit constexpr DoubleDataValue(std::int16_t value) : value_{value} {
    assert(kMin <= value);
  }

  /** Returns [0, 16383] integer value. */
  constexpr std::int16_t value() const { return value_; }

  /** Returns Least Significant Byte (LSB), i.e. value of lower 7 bits. */
  constexpr std::uint8_t lsb() const {
    return static_cast<std::uint8_t>(
        static_cast<std::uint16_t>(value_) & kLsbBitmask);
  }

  /** Returns Most Significant Byte (MSB), i.e. value of upper 7 bits. */
  constexpr std::uint8_t msb() const {
    return static_cast<std::uint8_t>(
        (static_cast<std::uint16_t>(value_) & kMsbBitmask) >> kDataBitsPerByte);
  }

  // Comparison operations:
  friend constexpr bool operator==(DoubleDataValue lhs, DoubleDataValue rhs) { return lhs.value_ == rhs.value_; }
  friend constexpr bool operator!=(DoubleDataValue lhs, DoubleDataValue rhs) { return lhs.value_ != rhs.value_; }
  friend constexpr bool operator<(DoubleDataValue lhs, DoubleDataValue rhs) { return lhs.value_ < rhs.value_; }
  friend constexpr bool operator<=(DoubleDataValue lhs, DoubleDataValue rhs) { return lhs.value_ <= rhs.value_; }
  friend constexpr bool operator>(DoubleDataValue lhs, DoubleDataValue rhs) { return lhs.value_ > rhs.value_; }
  friend constexpr bool operator>=(DoubleDataValue lhs, DoubleDataValue rhs) { return lhs.value_ >= rhs.value_; }

private:
  static constexpr std::int16_t kMin = 0;
  static constexpr std::int16_t kMidpoint = 8192;
  static constexpr std::int16_t kMax = 16383;

  static constexpr int kDataBitsPerByte = 7;
  static constexpr std::uint16_t kLsbBitmask = 0b0000'0000'0111'1111;
  static constexpr std::uint16_t kMsbBitmask = 0b0011'1111'1000'0000;

  std::int16_t value_;
};

}  // namespace bmmidi

#endif  // BMMIDI_DATA_VALUE_HPP
