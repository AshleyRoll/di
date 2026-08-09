#pragma once

#include <array>
#include <concepts>
#include <meta>
#include <ranges>
#include <string_view>
#include <vector>


namespace di::detail {
  template<std::unsigned_integral auto I>
  consteval static auto num_to_chars() -> std::string_view
  {
    constexpr auto Measure = [] -> int {
      auto val = I;
      int count = 1;
      while (val > 0) {
        val /= 10;
        if (val > 0) { ++count; }
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

  // build the fully qualified name of a type from its meta::info, removing all aliases so
  // the underlying type will always generate the same name
  consteval static auto dealiased_full_name(std::meta::info const& src_type) -> std::string_view
  {
    std::vector<std::string_view> parts{};
    auto type = dealias(src_type);

    parts.push_back(identifier_of(type));

    while (has_parent(type)) {
      type = dealias(parent_of(type));
      if (has_identifier(type)) { // top level namespace has no name, also anonymous namespaces
        parts.push_back(identifier_of(type));
      }
    }

    return std::define_static_string(parts | std::views::reverse | std::views::join_with(std::string_view{"::"}));
  }


  // build the fully qualified name of a type, removing all aliases so that the underlying
  // type will always generate the same name.
  template<typename T>
  consteval static auto dealiased_full_name() -> std::string_view {
    static_assert(std::is_class_v<T>, "Must be a class or struct");

    // force names to be de-aliased so that they will still match if requested by an alias
    return dealiased_full_name(^^T);
  }


}  // namespace di::detail