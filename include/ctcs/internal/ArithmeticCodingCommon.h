#pragma once

#include "ArithmeticCoding.h"

#include <utility>

namespace ctcs::ArithmeticCoding_NS
{

// Maps the given subinterval of [0; 1) into the specified interval [a; b).
// \param [lower_bound; upper_bound) - the input interval [a; b).
// \param [lower_bound_for_char/scaling_factor; upper_bound_for_char/scaling_factor) -
//        the desired subinterval of [0; 1).
// Expects that:
//   0 <= lower_bound < upper_bound <= ArithmeticCodingTraits::kTopValue.
//   0 <= lower_bound_for_char < upper_bound_for_char <= scaling_factor.
constexpr
std::pair<ArithmeticCodingTraits::CodeValue, ArithmeticCodingTraits::CodeValue>
getSubInterval(ArithmeticCodingTraits::CodeValue lower_bound,
               ArithmeticCodingTraits::CodeValue upper_bound,
               ArithmeticCodingTraits::FrequencyCount lower_bound_for_char,
               ArithmeticCodingTraits::FrequencyCount upper_bound_for_char,
               ArithmeticCodingTraits::FrequencyCount scaling_factor)
{
  const ArithmeticCodingTraits::CodeValue interval_length = upper_bound - lower_bound + 1;
  const ArithmeticCodingTraits::CodeValue lower_bound_new =
    lower_bound + lower_bound_for_char * interval_length / scaling_factor;
  const ArithmeticCodingTraits::CodeValue upper_bound_new =
    lower_bound + upper_bound_for_char * interval_length / scaling_factor - 1;
  return { lower_bound_new , upper_bound_new };
}

}
