#pragma once

#include "../exceptions.hpp"
#include "../inject.hpp"
#include "helpers.hpp"
#include "interfaces.hpp"

#include <memory>
#include <meta>
#include <ranges>
#include <vector>

namespace di::detail {

  template<typename T, typename I, inject<I> T::*Member>
  struct get_dependency
  {
    static void getter(container_interface &container, T *obj)
    {
      constexpr auto injected_name = dealiased_full_name<I>();
      auto registration = container.get_by_name(injected_name);
      if (not registration) {
        constexpr auto t_name = dealiased_full_name<T>();
        throw di::dependency_not_registered{ t_name, injected_name };
      }
      // TODO: enforce scope dependency rules

      (obj->*Member).m_ptr =  std::static_pointer_cast<I>(registration->get(container));
    };
  };

  // Handles building a constructor that we use to create an instance of type in the container
  // and injecting all the di::inject<> members
  template<typename T>
  struct constructor
  {
    // Test if a member is based on di::inject<>
    consteval static auto is_injected(std::meta::info member) -> bool
    {
      auto memberType = type_of(member);
      auto injectType = ^^di::inject;
      return has_template_arguments(memberType) and template_of(memberType) == injectType;
    }

    constexpr static auto injected_members = define_static_array(
      nonstatic_data_members_of(^^T, std::meta::access_context::unchecked()) | std::views::filter(is_injected));

    static constexpr auto injectors = [] {
      std::vector<auto (*)(container_interface &container, T *obj)->void> injectors;
      template for (constexpr auto m : injected_members)
      {
        constexpr auto member_type = type_of(m);
        constexpr auto injected_type = *template_arguments_of(member_type).begin();
        injectors.push_back(get_dependency<T, typename[:injected_type:], &[:m:]>::getter);
      }
      return define_static_array(injectors);
    }();

    // The constructor to use to make the T
    template<typename... Args>
    static auto make([[maybe_unused]] container_interface &container, Args &&...args) -> std::shared_ptr<T>
    {
      auto value = std::make_shared<T>(std::forward<Args>(args)...);

      template for (auto i : injectors) { i(container, value.get()); }

      return value;
    }
  };
}  // namespace di::detail