#pragma once

#include "detail/helpers.hpp"
#include "detail/interfaces.hpp"
#include "detail/registration.hpp"
#include "exceptions.hpp"
#include "provider.hpp"

#include <memory>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace di {


  class root_container final : public detail::container_interface
  {
  public:
    explicit root_container(
      std::unordered_map<std::string_view, std::unique_ptr<detail::registration_interface>> &&registrations)
      : m_registrations(std::move(registrations))
    {}

    auto get_by_name(std::string_view name) -> detail::registration_interface * override
    {
      if (not m_registrations.contains(name)) {
        return nullptr;
      }

      return m_registrations.at(name).get();
    }

  private:
    std::unordered_map<std::string_view, std::unique_ptr<detail::registration_interface>> m_registrations;
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

      m_registrations.emplace(full_name,
        std::make_unique<detail::persistent_registration<scope::Singleton, std::remove_cvref_t<T>, Args...>>(
          std::forward<Args>(args)...));

      return *this;
    }


    auto build() -> provider { return provider{ std::make_shared<root_container>(std::move(m_registrations)) }; }

  private:
    std::unordered_map<std::string_view, std::unique_ptr<detail::registration_interface>> m_registrations;
  };


}  // namespace di