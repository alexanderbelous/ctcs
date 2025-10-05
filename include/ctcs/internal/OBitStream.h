#pragma once

#include <stdexcept>
#include <string>

namespace ctcs::ArithmeticCoding_NS
{

// Wrapper for std::string, which writes data bit by bit.
class OBitStream
{
public:
  // Constructs an empty stream.
  // An empty OBitStream is not associated with any buffer. The stream will still keep track
  // of the number of bytes written.
  //constexpr OBitStream() noexcept = default;

  // Construct a wrapper for the given buffer.
  constexpr explicit OBitStream(std::string& dest) noexcept;

  // Non-copyable, non-movable.
  OBitStream(const OBitStream&) = delete;
  OBitStream(OBitStream&& other) = delete;
  OBitStream& operator=(const OBitStream&) = delete;
  OBitStream& operator=(OBitStream&& other) = delete;

  // Destructor.
  //
  // Calls finalize() before destroying the object. Note that if finalize() throws an exception,
  // it is suppressed in the destructor.
  constexpr ~OBitStream();

  // Writes the specified bit to the underlying ostream.
  //
  // Does nothing if there is no underlying stream.
  // Basic exception guarantee: if an exception is thrown during this operation, then the bit is not written.
  // \param bit - bit to write.
  constexpr void put(bool bit);

  // Writes uncommited bits to the underlying ostream.
  //
  // This is a potentially throwing operation, so this function is not marked noexcept.
  // Basic exception guarantee: if an exception is thrown during this operation, then the
  // associated stream and the uncommited bits remain the same.
  // If no exceptions occur, then stream() will return nullptr after this call.
  //
  // Does nothing if there's no associated ostream.
  constexpr void finalize();

private:
  // Bytes are always written with only 8 bits, even if CHAR_BIT > 8 on this platform.
  static constexpr unsigned char kNumBitsInByte = 8;

  std::string& dest_;
  // Stores the bits that were added via put() but haven't been written to dest_ yet.
  unsigned char current_byte_ = 0;
  // The number of bits written to current_byte_.
  unsigned char num_bits_ = 0;
};

constexpr OBitStream::OBitStream(std::string& dest) noexcept :
  dest_(dest)
{
}

constexpr OBitStream::~OBitStream()
{
  try
  {
    finalize();
  }
  catch (...)
  {
  }
}

constexpr void OBitStream::put(bool bit)
{
  // If the buffer is full, write the byte to output stream.
  if (num_bits_ == kNumBitsInByte)
  {
    dest_.push_back(static_cast<char>(current_byte_));
    current_byte_ = 0;
    num_bits_ = 0;
  }
  // Append the bit to the current byte.
  current_byte_ <<= 1;
  current_byte_ |= static_cast<unsigned char>(bit);
  ++num_bits_;
}

constexpr void OBitStream::finalize()
{
  if (num_bits_ == 0)
  {
    return;
  }
  const unsigned char last_byte = current_byte_ << (kNumBitsInByte - num_bits_);
  dest_.push_back(static_cast<char>(last_byte));
  current_byte_ = 0;
  num_bits_ = 0;
}

}
