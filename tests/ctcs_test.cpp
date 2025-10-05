#include <ctcs/ctcs.h>

#include <iostream>

// Any decent compiler will detect that the built-in string literals in this file
// are not used at runtime, and will compile them out.

namespace
{
  // CompressedString<ArithmeticCodingDecompressor<EnglishCharModel>, 11>
  constexpr ctcs::CompressedString kHelloWorldCompressed = ctcs::compress<"Hello, World!">();

  static_assert(sizeof(kHelloWorldCompressed) == 11,
    "Currently, the defaul compressor compresses \"Hello, World!\" into 11 bytes.");

  static_assert(kHelloWorldCompressed.decompress() == "Hello, World!",
    "If Decompressor is constexpr, then you can even verify at compile time "
    "that the decompressed string equals the original one.");
}

int main()
{
  // Decompress at runtime.
  std::string hello_world_decompressed = kHelloWorldCompressed.decompress();

  // CompressedString is implicitly convertible to std::string - the result is the decompressed string.
  std::string hello_world2 = ctcs::compress<"Hello, World!">();

  // Compress an empty string, brilliant.
  constexpr ctcs::CompressedString kEmptyString = ctcs::compress<"">();
  static_assert(kEmptyString.decompress().empty());

  // FYI: The Shadow Over Innsmouth is in public domain.
  constexpr ctcs::CompressedString kShadowOverInnsmouthCompressed = ctcs::compress<
    "The Shadow Over Innsmouth\n"
    "By H.P.Lovecraft\n"
    "\n"
    "------ = -O -= ------\n"
    "\n"
    "I.\n"
    "\n"
    "During the winter of 1927-28 officials of the Federal government made a "
    "strange and secret investigation of certain conditions in the ancient "
    "Massachusetts seaport of Innsmouth. The public first learned of it in "
    "February, when a vast series of raids and arrests occurred, followed by "
    "the deliberate burning and dynamiting - under suitable precautions - of "
    "an enormous number of crumbling, worm-eaten, and supposedly empty houses "
    "along the abandoned waterfront. Uninquiring souls let this occurrence "
    "pass as one of the major clashes in a spasmodic war on liquor.\n"
    "Keener news - followers, however, wondered at the prodigious number of "
    "arrests, the abnormally large force of men used in making them, and the "
    "secrecy surrounding the disposal of the prisoners.No trials, or even "
    "definite charges, were reported; nor were any of the captives seen "
    "thereafter in the regular gaols of the nation.There were vague "
    "statements about disease and concentration camps, and later about "
    "dispersal in various naval and military prisons, but nothing positive "
    "ever developed.Innsmouth itself was left almost depopulated, and is even "
    "now only beginning to shew signs of a sluggishly revived existence."
  >();
  static_assert(sizeof(kShadowOverInnsmouthCompressed) == 711);

  // Decompress at runtime.
  static const std::string shadow_over_innsmouth = kShadowOverInnsmouthCompressed.decompress();
  std::cout << shadow_over_innsmouth << std::endl;
  return 0;
}
