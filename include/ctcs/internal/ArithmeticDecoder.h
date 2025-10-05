#pragma once

#include "ArithmeticCoding.h"
#include "ArithmeticCodingCommon.h"
#include "IBitStream.h"

#include <stdexcept>
#include <utility>

namespace ctcs::ArithmeticCoding_NS
{

class ArithmeticDecoder
{
public:
  using CodeValue = ArithmeticCodingTraits::CodeValue;
  using FrequencyCount = ArithmeticCodingTraits::FrequencyCount;

  explicit constexpr ArithmeticDecoder(IBitStream& input_stream);

  template<ArithmeticCodingModel Model>
  constexpr typename Model::char_type decode(const Model& model);

private:
  constexpr void decodeImpl(std::pair<FrequencyCount, FrequencyCount> range, FrequencyCount denominator);

  constexpr void normalize();

  constexpr bool readBit();

  IBitStream& bit_stream_;
  // If the stream gets empty, we can still pretend that a few 0s are available.
  // However, we want to ensure that at least the highest 2 bits of the code value
  // are meaningful, so we track the number of such "garbage" bits.
  std::size_t num_garbage_bits_ = 0;
  // Currently-seen code value.
  CodeValue value_ = 0;
  // Ends of the current code region.
  CodeValue lower_bound_ = 0;
  CodeValue upper_bound_ = ArithmeticCodingTraits::kTopValue;
};

constexpr ArithmeticDecoder::ArithmeticDecoder(IBitStream& input_stream) :
  bit_stream_(input_stream)
{
  // Read the first bits to fill the code value.
  for (unsigned int i = 0; i < ArithmeticCodingTraits::kCodeValueBits; ++i)
  {
    value_ <<= 1;
    value_ |= static_cast<CodeValue>(readBit());
  }
}

template<ArithmeticCodingModel Model>
constexpr typename Model::char_type ArithmeticDecoder::decode(const Model& model)
{
  normalize();
  const CodeValue range = upper_bound_ - lower_bound_ + 1;
  const FrequencyCount scaling_factor = model.scalingFactor();
  const FrequencyCount cumulative_freq =
    static_cast<FrequencyCount>(((value_ - lower_bound_ + 1) * scaling_factor - 1) / range);
  // TODO: consider merging this into a single call.
  const Model::char_type symbol = model.getCharByPoint(cumulative_freq);
  const std::pair<FrequencyCount, FrequencyCount> interval = model.getInterval(symbol);
  decodeImpl(interval, scaling_factor);
  return symbol;
}

constexpr void ArithmeticDecoder::decodeImpl(std::pair<FrequencyCount, FrequencyCount> range, FrequencyCount denominator)
{
  // Narrow the code region to that alloted to this symbol.
  std::tie(lower_bound_, upper_bound_) =
    getSubInterval(lower_bound_, upper_bound_, range.first, range.second, denominator);
}

constexpr void ArithmeticDecoder::normalize()
{
  // Loop to get rid of bits.
  while (true)
  {
    if (upper_bound_ < ArithmeticCodingTraits::kHalf)
    {
      // nothing
    }
    else if (lower_bound_ >= ArithmeticCodingTraits::kHalf)
    {
      value_ -= ArithmeticCodingTraits::kHalf;
      lower_bound_ -= ArithmeticCodingTraits::kHalf;
      upper_bound_ -= ArithmeticCodingTraits::kHalf;
    }
    else if (lower_bound_ >= ArithmeticCodingTraits::kFirstQuarter &&
             upper_bound_ < ArithmeticCodingTraits::kThirdQuarter)
    {
      value_ -= ArithmeticCodingTraits::kFirstQuarter;
      lower_bound_ -= ArithmeticCodingTraits::kFirstQuarter;
      upper_bound_ -= ArithmeticCodingTraits::kFirstQuarter;
    }
    else
    {
      break;
    }
    lower_bound_ = 2 * lower_bound_;
    upper_bound_ = 2 * upper_bound_ + 1;
    value_ = 2 * value_ + static_cast<CodeValue>(readBit());
  }
}

constexpr bool ArithmeticDecoder::readBit()
{
  constexpr std::size_t kMaxGarbageBits = ArithmeticCodingTraits::kCodeValueBits - 2;
  if (std::optional<bool> bit = bit_stream_.get())
  {
    return *bit;
  }
  // ArithmeticCoder ensures that the output number of bits is sufficient for decoding
  // all characters from the input text. It is the responsibility of the user to know
  // when to stop calling decode() by tracking the number of characters decoded.
  ++num_garbage_bits_;
  if (num_garbage_bits_ > kMaxGarbageBits) {
    throw std::logic_error("ArithmeticDecoder: unexpected end of stream.");
  }
  return false;
}

}
