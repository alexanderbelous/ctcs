#pragma once

#include "internal/ArithmeticCoder.h"
#include "internal/ArithmeticDecoder.h"
#include "internal/VarInt.h"

namespace ctcs
{
  template<ArithmeticCoding_NS::ArithmeticCodingModel Model>
  class ArithmeticCodingDecompressor
  {
  public:
    constexpr void operator()(std::string_view compressed_data, std::string& dest)
    {
      // Special case: empty string is decompressed into an empty string.
      if (compressed_data.empty())
      {
        return;
      }
      // Read the size of the decompressed data.
      const Detail_NS::ReadVarIntResult read__size_result = Detail_NS::readVarInt(compressed_data);
      const std::size_t decompressed_data_size = read__size_result.value;
      // Discard the bytes we've just read.
      compressed_data = compressed_data.substr(read__size_result.num_bytes_read);
      dest.reserve(dest.size() + decompressed_data_size);
      ArithmeticCoding_NS::IBitStream bit_stream(compressed_data.data(), compressed_data.size());
      ArithmeticCoding_NS::ArithmeticDecoder decoder(bit_stream);
      Model model {};
      for (std::size_t i = 0; i < decompressed_data_size; ++i)
      {
        const char c = decoder.decode(model);
        dest.push_back(c);
        // TODO: update the model if it's adaptive.
      }
    }
  };

  // Compile-time compressor that uses arithmetic coding.
  // \param Model - ArithmeticCodingModel to use. 
  template<ArithmeticCoding_NS::ArithmeticCodingModel Model>
  class ArithmeticCodingCompressor
  {
  public:
    static_assert(std::is_default_constructible_v<Model>, "Model should be default-constructible.");
    static_assert(std::is_same_v<typename Model::char_type, char>, "Model::char_type should be char.");

    using Decompressor = ArithmeticCodingDecompressor<Model>;

    constexpr std::string operator()(std::string_view data)
    {
      // Special case: empty string is compressed into an empty string.
      if (data.empty())
      {
        return {};
      }
      std::string compressed_data;
      // Serialize the size of the uncompressed data.
      Detail_NS::writeVarInt(compressed_data, data.size());
      ArithmeticCoding_NS::OBitStream bit_stream(compressed_data);
      ArithmeticCoding_NS::ArithmeticCoder coder(bit_stream);
      Model model {};
      for (char c : data)
      {
        coder.encode(model, c);
        // TODO: update hte model if it's adaptive.
      }
      coder.finalize();
      bit_stream.finalize();
      return compressed_data;
    }
  };
}