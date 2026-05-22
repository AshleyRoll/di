#pragma once

#include "../inject.hpp"

#include <concepts>
#include <meta>
#include <ranges>
#include <string_view>

namespace di::detail {

  // always generates a new
  template<typename T, typename Container>
  struct constructor
  {
    consteval static auto is_injected(std::meta::info member) -> bool
    {
      auto member_type = type_of(member);
      auto inject_type = ^^di::inject;
      return has_template_arguments(member_type) and template_of(member_type) == inject_type;
    }

    static constexpr auto injected_members = define_static_array(
      nonstatic_data_members_of(^^T, std::meta::access_context::unchecked()) | std::views::filter(is_injected));

    // build a lamda that will complete the object by filling in all the inject<> members.
    consteval static auto make_injector()
    {
      return [](Container &container, T &value) consteval {
        template for (constexpr auto m : injected_members)
        {
          constexpr auto member_type = type_of(m);
          constexpr auto injected_type = template_arguments_of(member_type);
          value.[:m:].m_ptr = container.template get<injected_type>();
          // TODO: add deleter if needed
        }
      };
    }

    template<typename... Args>
    constexpr T make(Container &container, Args &&...args)
    {
      constexpr auto injector = make_injector();
      T value{ std::forward<Args>(args)... };
      injector(container, value);
      return value;
    }
  };

}  // namespace di::detail