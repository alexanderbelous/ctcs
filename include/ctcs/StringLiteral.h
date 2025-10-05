#pragma once

#include <cstddef>
#include <string_view>

namespace ctcs
{
  // String-like class whose objects can only be constructed at compile time.
  // TODO: specialize for Length==0
  template<std::size_t Length>
  class StringLiteral
  {
  public:
    // Constructs a zero-initialized StringLiteral.
    constexpr StringLiteral() noexcept:
      data{}
    {}

    // Constructs a StringLiteral from a built-in string literal.
    // Note that the last character - the null terminator - is not copied.
    // \param str - input string literal.
    constexpr StringLiteral(const char (&str)[Length + 1]) noexcept
    {
      for (std::size_t i = 0; i < Length; ++i)
      {
        data[i] = str[i];
      }
    }

    // \return the size of the string.
    constexpr std::size_t size() const noexcept
    {
      return Length;
    }

    // \return a std::string_view into this string.
    constexpr std::string_view view() const noexcept
    {
      return std::string_view(data, Length);
    }

    char data[Length];
  };

  // Specialization for Length==0.
  template<>
  class StringLiteral<0>
  {
  public:
    // Constructs a zero-initialized StringLiteral.
    constexpr StringLiteral() noexcept = default;

    // Constructs a StringLiteral from a built-in string literal.
    // Note that the last character - the null terminator - is not copied.
    // \param str - input string literal.
    constexpr StringLiteral(const char(&)[1]) noexcept
    {
    }

    // \return the size of the string.
    constexpr std::size_t size() const noexcept
    {
      return 0;
    }

    // \return a std::string_view into this string.
    constexpr std::string_view view() const noexcept
    {
      return {};
    }

    char data[1]{};
  };

  // Deduction guides for StringLiteral.
  template <std::size_t N> StringLiteral(const char(&)[N]) -> StringLiteral<N - 1>;
  template <std::size_t N> StringLiteral(StringLiteral<N>) -> StringLiteral<N>;
}
