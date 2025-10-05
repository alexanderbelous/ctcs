#pragma once

#include "StringLiteral.h"

#include <cstddef>
#include <string>

// ctcs = Compile-time Compressed String.
namespace ctcs
{
  // Wrapper for StringLiteral.
  // \param Decompressor - class that should be used to decompress the data.
  // \param CompressedLength - size of the compressed data in bytes.
  template<class Decompressor, std::size_t CompressedLength>
  class CompressedString
  {
  public:
    explicit constexpr CompressedString(StringLiteral<CompressedLength> compressed_data) noexcept:
      compressed_data_(compressed_data)
    {
    }

    // Decompresses the data into std::string.
    constexpr std::string decompress() const
    {
      std::string result;
      decompress(result);
      return result;
    }

    // Decompresses the data and appends it to the given std::string.
    constexpr void decompress(std::string& dest) const
    {
      Decompressor{}(compressed_data_.view(), dest);
    }

    // Implicit conversion to std::string.
    // \return the decompressed string.
    constexpr operator std::string() const
    {
      return decompress();
    }

    constexpr const StringLiteral<CompressedLength>& compressedData() const noexcept
    {
      return compressed_data_;
    }

  private:
    StringLiteral<CompressedLength> compressed_data_;
  };

  // Deduction guides for CompressedString.
  template <class Decompressor, std::size_t N> CompressedString(CompressedString<Decompressor, N>)
    -> CompressedString<Decompressor, N>;
}
