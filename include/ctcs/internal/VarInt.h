#pragma once

#include <cstdint>
#include <string>

namespace ctcs::Detail_NS
{
  // Serializes the given number as VarInt.
  // \param dest - output string. Serialized data will be appended to the string.
  // \param value - number to serialize.
  constexpr void writeVarInt(std::string& dest, std::uintmax_t value)
  {
    // The number of bits in a byte of a VarInt.
    constexpr unsigned int kNumBitsInByte = 8;
    // The number of payload bits per "byte" of a VarInt.
    constexpr unsigned int kNumPayloadBits = kNumBitsInByte - 1;
    // Bit mask for the payload bits.
    constexpr std::uintmax_t kPayloadBitsMask = (1 << kNumPayloadBits) - 1;
    // Bit mask for the MSB.
    constexpr unsigned char kSentinelBitMask = 1 << kNumPayloadBits;

    do
    {
      // Get the least significant payload bits.
      unsigned char bits = static_cast<unsigned char>(value & kPayloadBitsMask);
      // Shift right, discarding the least significant payload bits.
      value >>= kNumPayloadBits;
      // If there are more nonzero bits, set the sentinel bit to 1, otherwise leave it 0.
      if (value > 0)
      {
        bits |= kSentinelBitMask;
      }
      // Append the artificial "byte".
      dest.push_back(static_cast<char>(bits));
    } while (value > 0);
  }

  struct ReadVarIntResult
  {
    std::uintmax_t value;
    std::size_t num_bytes_read;
  };

  // Deserializes a VarInt from the input string.
  // \param data - input string.
  // \return the deserialized integer and the number of consumed bytes.
  // \throw std::out_of_range if failed to deserialize the input data as VarInt
  //        (i.e. if the terminal byte is missing from @data).
  constexpr ReadVarIntResult readVarInt(std::string_view data)
  {
    // The number of bits in a byte of a VarInt.
    constexpr unsigned int kNumBitsInByte = 8;
    // The number of payload bits per "byte" of a VarInt.
    constexpr unsigned int kNumPayloadBits = kNumBitsInByte - 1;
    // Bit mask for the least significant (kNumBitsInByte-1) bits.
    constexpr unsigned char kPayloadBitsMask = (1 << kNumPayloadBits) - 1;
    // Bit mask for the least significant kNumBitsInByte-th bit.
    constexpr unsigned char kSentinelBitMask = 1 << kNumPayloadBits;

    std::uintmax_t result = 0;
    std::uintmax_t shift = 0;
    bool has_more_bytes = true;
    std::size_t num_bytes_read = 0;
    do
    {
      // Read a single "byte" of a VarInt.
      const unsigned char bits = data.at(num_bytes_read);
      ++num_bytes_read;
      // Check the sentinel bit, which tells if there are more bytes to read.
      has_more_bytes = bits & kSentinelBitMask;
      // Set the sentinel bit to 0, leaving only the payload bits.
      const unsigned char payload_bits = bits & kPayloadBitsMask;
      // Update the result
      result |= (static_cast<std::uintmax_t>(payload_bits) << shift);
      // Update the shift for the next byte.
      shift += kNumPayloadBits;
    } while (has_more_bytes);
    return ReadVarIntResult{ .value = result, .num_bytes_read = num_bytes_read };
  }
}
