#pragma once

#include "detail/helpers.hpp"
#include "detail/interfaces.hpp"
#include "detail/registration.hpp"
#include "exceptions.hpp"

#include <memory>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace di {

  // The provider class is the abstract interface to the di::container
  // this allows the DI container to be defined in a separate TU and the consumer
  // only needing to know the types it specifically needs to instantiate, providing a nice
  // compile time fire-wall.
  //
  // The lifespan of this container and all its copies will define the lifespan of the di::container
  // This means that the user must ensure this object is live until all possible clients of the di are
  // destroyed.
  class provider
  {
  public:
    explicit constexpr provider(std::shared_ptr<detail::container_interface> &&container)
      : m_container(std::move(container))
    {}

    // resolve a type and return an instance wrapped in a std::shared_ptr.
    template<typename T>
      requires std::is_class_v<std::remove_cvref_t<T>>
    constexpr auto get() -> std::shared_ptr<std::remove_cvref_t<T>>
    {
      return m_container->get<T>();
    }

  private:
    std::shared_ptr<detail::container_interface> m_container;
  };


  // The root container for the DI provider.
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

  // A fluent-style dependency registration builder.
  //
  // Use this class to register your dependencies. It can be passed to other libraries for
  // them to add their registrations and still maintain a compiler firewall for types.
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

    template<typename T, typename... Args>
      requires std::is_class_v<std::remove_cvref_t<T>> and std::is_constructible_v<T, Args...>
    auto add_transient(Args &&...args) -> builder &
    {
      constexpr auto full_name = detail::dealiased_full_name<std::remove_cvref_t<T>>();
      if (m_registrations.contains(full_name)) { throw di::type_already_registered{ full_name }; }

      m_registrations.emplace(full_name,
        std::make_unique<detail::transient_registration<std::remove_cvref_t<T>, Args...>>(
          std::forward<Args>(args)...));

      return *this;
    }

    // Build the final provider for the DI system. This is is the root of the dependency injector.
    // The returned provider manages the lifetime of all dependencies.
    auto build() -> provider { return provider{ std::make_shared<root_container>(std::move(m_registrations)) }; }

  private:
    std::unordered_map<std::string_view, std::unique_ptr<detail::registration_interface>> m_registrations;
  };


}  // namespace di