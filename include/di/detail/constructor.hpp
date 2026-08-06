#pragma once

#include "../inject.hpp"

#include <meta>
#include <ranges>
#include <memory>

namespace di::detail {

  // Handles building a constructor that we use to create an instance of type in the container
  // and injecting all the di::inject<> members
  template<typename T, typename Container>
  struct constructor
  {

    // Test if a member is based on di::inject<>
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
      return [](Container *container, T &value) -> auto {
        template for (constexpr auto m : injected_members)
        {
          constexpr auto member_type = type_of(m);

          value.[:m:].m_ptr = container->template get<typename [:*template_arguments_of(member_type).begin():]>();
        }
      };
    }

    // The constructor to use to make the T
    template<typename... Args>
    static auto make(Container *container, Args &&...args) -> std::unique_ptr<T>
    {
      constexpr auto injector = make_injector();
      auto value = std::make_unique<T>(std::forward<Args>(args)...);
      injector(container, *value.get());
      return value;
    }
  };

}  // namespace di::detail