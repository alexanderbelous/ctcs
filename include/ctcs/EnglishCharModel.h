#pragma once

#include "internal/ArithmeticCoding.h"

#include <algorithm>
#include <array>
#include <span>
#include <stdexcept>

namespace ctcs
{
  namespace Detail_NS
  {
    template<class T, std::size_t N>
    constexpr std::array<T, N> makeFrequenciesSafe(std::span<const T, N> frequencies)
    {
      std::array<T, N> result{};
      for (std::size_t i = 0; i < N; ++i)
      {
        result[i] = std::max(frequencies[i], T{ 1 });
      }
      return result;
    }

    template<class T, std::size_t N>
    constexpr std::array<T, N+1> makeCumulativeHistogram(std::span<const T, N> frequencies)
    {
      std::array<T, N + 1> result{};
      T partial_sum {};
      for (std::size_t i = 0; i < N; ++i)
      {
        result[i] = partial_sum;
        partial_sum += frequencies[i];
      }
      result[N] = partial_sum;
      return result;
    }
  }
  // ArithmeticCodingModel for encoding char elements.
  // Represents the distribution of characters in a typical English text.
  class EnglishCharModel
  {
    static constexpr std::uint32_t kNumCharacters = 256;

  public:
    using FrequencyCount = ArithmeticCoding_NS::ArithmeticCodingTraits::FrequencyCount;
    using char_type = char;

    constexpr FrequencyCount scalingFactor() const noexcept
    {
      return kCumulativeFrequencies[kNumCharacters];
    }

    // Find the character, whose interval [lower, upper) contains the specified value.
    constexpr char_type getCharByPoint(FrequencyCount value) const
    {
      // Iterator to the first element kCumulativeFrequencies[i] > value.
      const auto iter = std::upper_bound(kCumulativeFrequencies.begin(), kCumulativeFrequencies.end(), value);
      if (iter == kCumulativeFrequencies.end())
      {
        throw std::out_of_range("EnglishCharModel::getCharPoint(): value is out of range.");
      }
      const std::size_t index = iter - kCumulativeFrequencies.begin();
      return static_cast<char_type>(index - 1);
    }

    // Returns the interval for the specified character.
    constexpr std::pair<FrequencyCount, FrequencyCount> getInterval(char_type character) const
    {
      const std::size_t index = static_cast<std::size_t>(static_cast<unsigned char>(character));
      if (index >= kNumCharacters)
      {
        throw std::out_of_range("EnglishCharModel::getInterval(): character is out of range.");
      }
      return { kCumulativeFrequencies[index], kCumulativeFrequencies[index + 1] };
    }

  private:
    // The number of occurrences for each character in the Brown Corpus.
    static constexpr FrequencyCount kFrequencies[kNumCharacters] =
    {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 13580, 89324, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      1105892, 1597, 0, 0, 579, 147, 166, 28683, 2464, 2495, 173, 1, 58982, 15401, 55578, 236,
      4458, 5182, 2621, 1732, 1452, 2144, 1451, 1065, 1265, 2125, 1987, 5566, 0, 0, 0, 4694,
      0, 11385, 6527, 7776, 4080, 3166, 4263, 3444, 8015, 12543, 3008, 1494, 3252, 7455, 3798, 3267,
      5162, 241, 3663, 10322, 15568, 1640, 1055, 6003, 56, 1610, 122, 2, 0, 2, 0, 0,
      17674, 371418, 66277, 139434, 184215, 589980, 106409, 89140, 249219, 333212, 4748, 29685, 192894, 113186, 332908, 357020,
      90770, 4862, 287337, 300431, 423392, 127159, 46206, 83137, 9379, 80164, 4431, 16, 0, 16, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    static constexpr std::array<FrequencyCount, kNumCharacters + 1> kCumulativeFrequencies =
      Detail_NS::makeCumulativeHistogram(std::span<const FrequencyCount, kNumCharacters>(
        Detail_NS::makeFrequenciesSafe(std::span<const FrequencyCount, kNumCharacters>(kFrequencies))));
  };
}
