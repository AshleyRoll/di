#pragma once

#include <array>
#include <concepts>
#include <meta>
#include <string_view>


namespace di::detail {
  template<std::unsigned_integral auto I>
  consteval static auto num_to_chars() -> std::string_view
  {
    constexpr auto Measure = [] -> int {
      auto val = I;
      int count = 1;
      while (val > 0) {
        val /= 10;
        if(val > 0) {
          ++count;
        }
      }
      return count;
    };

    std::array<char, Measure()> chars{};

    auto val = I;
    for (int idx = chars.size() - 1; idx >= 0; --idx) {
      chars[idx] = static_cast<char>('0' + (val % 10));
      val /= 10;
    }

    return std::string_view{ std::define_static_string(chars) };
  }
}