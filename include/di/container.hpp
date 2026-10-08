#pragma once

#include "detail/container_interface.hpp"
#include "detail/helpers.hpp"
#include "detail/registration.hpp"
#include "exceptions.hpp"
#include "provider.hpp"

#include <cstdint>
#include <memory>
#include <meta>
#include <ranges>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace di {

  // The scope of an object defines when a new instance is created. An instance of earlier scope can not
  // reference an object of a later scope. That is a Singleton can only reference other Singletons, Scoped
  // can reference either a Singleton or another scoped object
  enum struct scope : std::uint8_t { Singleton, Scoped, Transient };


  class root_container final : public detail::container_interface
  {
  public:

  };


  class builder
  {
  public:
    template<typename T, typename... Args>
      requires std::is_class_v<std::remove_cvref_t<T>> and std::is_constructible_v<T, Args...>
    auto add_singleton(Args &&...args) -> builder &
    {
      constexpr auto full_name = detail::dealiased_full_name<std::remove_cvref_t<T>>();
      if (m_registrations.contains(full_name)) { throw di::type_already_registered{ full_name }; }

      auto reg = detail::registration<scope::Singleton, std::remove_cvref_t<T>, Args...>(std::forward<Args>(args)...);
      m_registrations.emplace(full_name, std::move(reg));

      return *this;
    }


    auto build() -> provider {}

  private:
    std::unordered_map<std::string_view, detail::registration_interface> m_registrations;
  };


}  // namespace di