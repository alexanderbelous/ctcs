#pragma once

#include "ArithmeticCoding.h"
#include "ArithmeticCodingCommon.h"
#include "OBitStream.h"

#include <cassert>
#include <stdexcept>

namespace ctcs::ArithmeticCoding_NS
{

// Arithmetic coding allows mutable models, as long as the changes are synchronized
// during encoding and decoding (e.g., as in adaptive arithmetic coding).
//
// In laconia I utilize this fact to the maximum - not only the frequencies will
// be updated, but the *alphabet itself* will change between calls to encode():
// fiirst, we encode the PartOfSpeech, then LexemeID, then InflectionForm, etc.
class ArithmeticCoder
{
public:
  using CodeValue = ArithmeticCodingTraits::CodeValue;
  using FrequencyCount = ArithmeticCodingTraits::FrequencyCount;

  explicit constexpr ArithmeticCoder(OBitStream& output_stream);

  // Non-copyable, non-movable.
  ArithmeticCoder(const ArithmeticCoder&) = delete;
  ArithmeticCoder(ArithmeticCoder&& other) = delete;
  ArithmeticCoder& operator=(const ArithmeticCoder&) = delete;
  ArithmeticCoder& operator=(ArithmeticCoder&& other) = delete;

  // Destructor.
  //
  // Calls finalize() under the hood. If finalize() throws any exception, it is suppressed in the destructor.
  constexpr ~ArithmeticCoder();

  template<ArithmeticCodingModel Model>
  constexpr void encode(const Model& model, typename Model::char_type symbol);

  constexpr void encode(std::pair<FrequencyCount, FrequencyCount> range, FrequencyCount denominator);

  constexpr void finalize();

private:
  constexpr void pushBit(bool bit);

  static constexpr ArithmeticCodingTraits::CodeValue kFirstQuarter = ArithmeticCodingTraits::kFirstQuarter;
  static constexpr ArithmeticCodingTraits::CodeValue kHalf = ArithmeticCodingTraits::kHalf;
  static constexpr ArithmeticCodingTraits::CodeValue kThirdQuarter = ArithmeticCodingTraits::kThirdQuarter;

  // The output stream for encoded data.
  OBitStream* bit_stream_ = nullptr;
  // Left and right endpoints of the interval representing the current state.
  CodeValue lower_bound_ = 0;
  CodeValue upper_bound_ = ArithmeticCodingTraits::kTopValue;
  // The number of opposite bits to output after the next bit.
  unsigned int bits_to_follow_ = 0;
};

constexpr ArithmeticCoder::ArithmeticCoder(OBitStream& output_stream):
  bit_stream_(&output_stream)
{
}

constexpr ArithmeticCoder::~ArithmeticCoder()
{
  try
  {
    finalize();
  }
  catch (...)
  {
  }
}

constexpr void ArithmeticCoder::encode(std::pair<FrequencyCount, FrequencyCount> range, FrequencyCount denominator)
{
  assert(denominator != 0);
  assert(range.first < range.second);
  assert(range.second <= denominator);

  // Determine the new subinterval.
  std::tie(lower_bound_, upper_bound_) =
    getSubInterval(lower_bound_, upper_bound_, range.first, range.second, denominator);
  // Perform renormalization, if needed.
  while (true)
  {
    if (upper_bound_ < kHalf)
    {
      pushBit(0);
    }
    else if (lower_bound_ >= kHalf)
    {
      pushBit(1);
      lower_bound_ -= kHalf;
      upper_bound_ -= kHalf;
    }
    else if (kFirstQuarter <= lower_bound_ && upper_bound_ < kThirdQuarter)
    {
      bits_to_follow_++;
      lower_bound_ -= kFirstQuarter;
      upper_bound_ -= kFirstQuarter;
    }
    else
    {
      break;
    }
    lower_bound_ = lower_bound_ * 2;
    upper_bound_ = upper_bound_ * 2 + 1;
  }
}

constexpr void ArithmeticCoder::finalize()
{
  if (!bit_stream_)
  {
    return;
  }
  // Push 2 bits that select the quarter that the current code range contains.
  bits_to_follow_ += 1;
  const bool first_bit = (lower_bound_ < kFirstQuarter) ? 0 : 1;
  pushBit(first_bit);
  // Write the remaining bits (if any) to the output stream.
  //bit_stream_.finalize();
  bit_stream_ = nullptr;
}

constexpr void ArithmeticCoder::pushBit(bool bit)
{
  if (!bit_stream_)
  {
    throw std::runtime_error("ArithmeticCoder::pushBit(): finalize() has already been called.");
  }
  bit_stream_->put(bit);
  for (; bits_to_follow_ > 0; --bits_to_follow_)
  {
    bit_stream_->put(!bit);
  }
}

template<ArithmeticCodingModel Model>
constexpr void ArithmeticCoder::encode(const Model& model, typename Model::char_type symbol)
{
  encode(model.getInterval(symbol), model.scalingFactor());
}

}
