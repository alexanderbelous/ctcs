#pragma once

#include "ArithmeticCodingCompressor.h"
#include "CompressedString.h"
#include "EnglishCharModel.h"
#include "StringLiteral.h"

#include <cstddef>
#include <string>

// ctcs = Compile-time Compressed String.
namespace ctcs
{
  // Compresses the given string literal.
  // \param Str - input string.
  // \param Compressor - compressor to use.
  template<StringLiteral Str, class Compressor = ArithmeticCodingCompressor<EnglishCharModel>>
  consteval auto compress()
  {
    using Decompressor = typename Compressor::Decompressor;
    constexpr std::size_t kCompressedDataSize = Compressor{}(Str.view()).size();
    const std::string compressed_data = Compressor{}(Str.view());
    StringLiteral<kCompressedDataSize> compressed_data_literal;
    for (std::size_t i = 0; i < kCompressedDataSize; ++i)
    {
      compressed_data_literal.data[i] = compressed_data[i];
    }
    return CompressedString<Decompressor, kCompressedDataSize>(compressed_data_literal);
  }
}
