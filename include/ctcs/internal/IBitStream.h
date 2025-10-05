#pragma once

#include <optional>

namespace ctcs::ArithmeticCoding_NS
{

// Wrapper for std::span<const char>, which reads data bit by bit.
class IBitStream
{
public:
  // Constructs an empty stream.
  constexpr IBitStream() noexcept = default;

  // Constructs a stream.
  constexpr IBitStream(const char* stream, std::size_t num_characters) noexcept;

  // Non-copyable.
  IBitStream(const IBitStream&) = delete;
  IBitStream& operator=(const IBitStream&) = delete;

  // Movable
  constexpr IBitStream(IBitStream&& other) noexcept;
  constexpr IBitStream& operator=(IBitStream&& other) noexcept;

  constexpr ~IBitStream() = default;

  // Read 1 bit from the underlying stream.
  // \return the value of the bit, or std::nullopt if the end of the stream has been reached.
  constexpr std::optional<bool> get();

private:
  // Pointer to the next character in the stream.
  const char* stream_ = nullptr;
  // Pointer past the last character in the stream.
  const char* stream_end_ = nullptr;
  // Current character.
  unsigned char buffer_ = 0;
  // The number of bits in buffer_ that haven't been read yet.
  unsigned char num_bits_in_buffer_ = 0;
};

constexpr IBitStream::IBitStream(const char* stream, std::size_t num_characters) noexcept:
  stream_(stream),
  stream_end_(stream ? stream + num_characters : nullptr)
{}

constexpr IBitStream::IBitStream(IBitStream&& other) noexcept:
  stream_(other.stream_),
  stream_end_(other.stream_end_),
  buffer_(other.buffer_),
  num_bits_in_buffer_(other.num_bits_in_buffer_)
{
  other.stream_ = nullptr;
  other.stream_end_ = nullptr;
  other.buffer_ = 0;
  other.num_bits_in_buffer_ = 0;
}

constexpr IBitStream& IBitStream::operator=(IBitStream&& other) noexcept
{
  stream_ = other.stream_;
  stream_end_ = other.stream_end_;
  buffer_ = other.buffer_;
  num_bits_in_buffer_ = other.num_bits_in_buffer_;
  other.stream_ = nullptr;
  other.stream_end_ = nullptr;
  other.buffer_ = 0;
  other.num_bits_in_buffer_ = 0;
  return *this;
}

constexpr std::optional<bool> IBitStream::get()
{
  constexpr std::size_t kNumBitsInByte = 8;

  if (num_bits_in_buffer_ == 0)
  {
    if (stream_ == stream_end_)
    {
      return std::nullopt;
    }
    buffer_ = static_cast<unsigned char>(*stream_++);
    num_bits_in_buffer_ = kNumBitsInByte;
  }
  return static_cast<bool>((buffer_ >> (--num_bits_in_buffer_)) & 1);
}

}
