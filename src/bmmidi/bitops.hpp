#ifndef BMMIDI_BITOPS_HPP
#define BMMIDI_BITOPS_HPP

#include <cstdint>

namespace bmmidi {
namespace internal {

/** Returns only the bits in value where bitMask has 1 bits. */
inline constexpr std::uint8_t getBits(std::uint8_t value, std::uint8_t bitMask) {
  return (value & bitMask);
}

/**
 * Returns oldValue with 1 bits in bitMask replaced by the corresponding bits in
 * newValue.
 */
inline constexpr std::uint8_t setBits(
    std::uint8_t oldValue, std::uint8_t newValue, std::uint8_t bitMask) {
  std::uint8_t result = oldValue;
  result &= ~bitMask;  // Clear the bitMask bits.
  result |= (newValue & bitMask);  // Replace with corresponding bits from newValue.
  return result;
}

}  // namespace internal
}  // namespace bmmidi

#endif  // BMMIDI_BITOPS_HPP
