#pragma once

#include <bit>
#include <climits>
#include <concepts>
#include <cstdint>
#include <limits>
#include <utility>

namespace ctcs::ArithmeticCoding_NS
{

class ArithmeticCodingTraits
{
public:
  // The number of bits in a code value.
  static constexpr unsigned int kCodeValueBits = 32;
  // Type of an arithmetic code value.
  using CodeValue = std::uint64_t;
  // Largest code value, 2^N-1.
  static constexpr CodeValue kTopValue = (static_cast<CodeValue>(1) << kCodeValueBits) - 1;
  // Half and quarter points in the code value range.
  // Point after the first quarter, 2^(N-2).
  static constexpr CodeValue kFirstQuarter = kTopValue / 4 + 1;
  // Point after the first half, 2^(N-1).
  static constexpr CodeValue kHalf = kFirstQuarter * 2;
  // Point after the third quarter, 3*2^(N-2).
  static constexpr CodeValue kThirdQuarter = kFirstQuarter * 3;
  // Type to store cumulative frequencies.
  using FrequencyCount = std::uint32_t;
  // The maximum allowed cumulative frequency.
  static constexpr FrequencyCount kMaxFrequency = (static_cast<FrequencyCount>(1) << 30) - 1;

  // Check the constraints:
  static_assert(kCodeValueBits > 2, "kCodeValueBits must be greater than 2.");
  static_assert(std::numeric_limits<CodeValue>::digits >= kCodeValueBits,
    "CodeValue must have enough bits to represent all code values.");
  static_assert(std::bit_width(kMaxFrequency) + 2 <= kCodeValueBits,
    "The integer range spanned by the cumulative frequencies must "
    "fit into a quarter of that provided by code values to prevent underflow.");
  static_assert(static_cast<std::uintmax_t>(kTopValue) * static_cast<std::uintmax_t>(kMaxFrequency)
    <= std::numeric_limits<CodeValue>::max(),
    "The product of kTopValue and kMaxFrequency must fit into CodeValue to prevent overflow.");
};

// Defines a named requirement for an arithmetic coding model.
template<typename T>
concept ArithmeticCodingModel = requires(const T& model)
{
  // T must have a member typedef 'char_type'.
  typename T::char_type;
  // 'T::char_type' must not be void.
  requires !std::is_same_v<typename T::char_type, void>;
  // If 'model' is a const-qualified reference to T,
  // then model.scalingFactor() must return FrequencyCount.
  {model.scalingFactor()} -> std::same_as<ArithmeticCodingTraits::FrequencyCount>;
  // If 'model' is a const-qualified reference to T,
  // then calling model.getCharByPoint() with a parameter of the type FrequencyCount
  // must return 'T::char_type'
  {model.getCharByPoint(std::declval<const ArithmeticCodingTraits::FrequencyCount&>())} -> std::same_as<typename T::char_type>;
  // If 'model' is a const-qualified reference to T,
  // then calling model.getInterval() with a parameter of the type T::char_type
  // must return std::pair<FrequencyCount, FrequencyCount>
  {model.getInterval(std::declval<const typename T::char_type&>())}
    -> std::same_as<std::pair<ArithmeticCodingTraits::FrequencyCount, ArithmeticCodingTraits::FrequencyCount>>;

  // Note that scalingFactor(), getCharByPoint(), getInterval() can be static member functions.
};

}
