#pragma once

#include "constructor.hpp"
#include "helpers.hpp"
#include "interfaces.hpp"

#include <functional>
#include <memory>
#include <string_view>
#include <tuple>
#include <utility>

namespace di::detail {

  template<di::scope S, typename T, typename... Args>
    requires(S == di::scope::Singleton or S == di::scope::Scoped)
  class persistent_registration final : public registration_interface
  {
  public:
    explicit persistent_registration(Args &&...args)
      : ctor_args{ std::forward<Args>(args)... }
    {}

    ~persistent_registration() override = default;
    auto type_name() -> std::string_view override { return detail::dealiased_full_name<T>(); }
    auto scope() -> di::scope override { return S; }

    // A stored instance returns a pointer to the shared_ptr
    auto get(container_interface &container) -> std::shared_ptr<void> override
    {
      if (not instance) {
        if constexpr (sizeof...(Args) > 0) {
          instance = std::move(std::apply(
            [&container]<typename... CtorArgs>(
              CtorArgs &&...args) -> auto { return constructor<T>::make(container, std::forward<CtorArgs>(args)...); },
            ctor_args));
        } else {
          instance = constructor<T>::make(container);
        }
      }

      return instance;
    }

    std::tuple<Args...> ctor_args;
    std::shared_ptr<T> instance{ nullptr };
  };

  template<typename T, typename... Args>
  class transient_registration final : public registration_interface
  {
  public:
    explicit transient_registration(Args &&...args)
      : ctor_args{ std::forward<Args>(args)... }
    {}

    ~transient_registration() override = default;
    auto type_name() -> std::string_view override { return detail::dealiased_full_name<T>(); }
    auto scope() -> di::scope override { return di::scope::Transient; }

    // A transient instance returns a pointer to a function to make an instance and taking a container_interface &
    auto get(container_interface &container) -> std::shared_ptr<void> override
    {
      if constexpr (sizeof...(Args) > 0) {
        return std::apply(
          [&container]<typename... CtorArgs>(
            CtorArgs &&...args) -> auto { return constructor<T>::make(container, std::forward<CtorArgs>(args)...); },
          ctor_args);
      } else {
        return constructor<T>::make(container);
      }
    }

    std::tuple<Args...> ctor_args;
  };


}  // namespace di::detail