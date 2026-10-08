#pragma once

#include "../container.hpp"
#include "../provider.hpp"
#include "constructor.hpp"
#include "helpers.hpp"

#include <memory>
#include <string_view>
#include <tuple>

namespace di::detail {
  struct registration_interface
  {
    virtual ~registration_interface() = default;

    virtual auto type_name() -> std::string_view = 0;
    virtual auto scope() -> di::scope = 0;

    // returns a type erased std::shared_ptr<T> of the type registered
    virtual auto get(provider &provider) -> void * = 0;
  };

  template<di::scope S, typename T, typename... Args>
  struct registration final : registration_interface
  {
    registration(Args &&...args)
      : args{ std::forward<Args>(args)... }
      , instance{ nullptr }
    {}

    ~registration() override = default;
    auto type_name() -> std::string_view override { return detail::dealiased_full_name<T>(); }
    auto scope() -> di::scope override { return S; }

    auto get(provider &provider) -> void * override
    {
      if (instance == nullptr) {
        instance = std::apply(
          [&provider](Args &&...args) { return constructor<T>::make(provider, std::forward<Args>(args)...); },
          args);
      }

      return &instance;
    }

    std::tuple<Args...> args;
    std::shared_ptr<T> instance;
  };
}  // namespace di::detail