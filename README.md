# ctcs
Header-only library for compressing strings at compile time.

Decompression at compile time is possible, but that defies the purpose: the goal is to reduce the binary size.

# Rationale
Fun.

# Usage
Consider better alternatives:
* Compress the binary itself.
* Use native filesystem compression.
* Store the strings in a resource file instead of compressing them individually.
* Use better compression algorithms.

Still not convinced? OK:

```cpp
#include <ctcs/ctcs>

std::string_view getBigString()
{
  // A decent compile will detect that the string literal is not used at runtime and can be compiled out.
  // The compressed string is decompressed at runtime the first time this function is called.
  static const std::string kStr = ctcs::compress<
    "All work and no play makes Jack a dull boy\n"
    "All work and no play makes Jack a dull boy\n"
    "All work and no play makes Jack a dull boy\n"
    "All work and no play makes Jack a dull boy\n"
    "All work and no play makes Jack a dull boy\n"
  >();
  // The string literal has 216 characters (including the null terminator).
  // The compressed string currently occupies 133 bytes.
  return kStr;
}
```
