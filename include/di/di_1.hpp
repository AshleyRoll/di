#pragma once

#include <meta>
#include <ranges>
#include <tuple>
#include <vector>
#include <optional>

namespace di {
  template<typename... Ts>
  struct type_list
  {
  };

  template<typename C, typename T>
  struct getter;

  template<typename C, typename T>
  struct getter
  {
    consteval static auto make_param_list(std::meta::info type)
    {
      return std::meta::substitute(^^type_list,
        std::meta::parameters_of(type)
        | std::views::transform(std::meta::type_of)
        | std::views::transform(std::meta::remove_cvref)
        );
    }

    template<typename... Ts>
    consteval static std::tuple<Ts...> get_args(type_list<Ts...> const &, C const &container)
    { return { getter<C,Ts>{}(container)... }; }

    template<typename... Ts>
    consteval static T make(std::tuple<Ts...> &&args)
    { return T{ std::get<Ts>(args)... }; }

    consteval static std::meta::info find_ctor(std::meta::info type)
    {
      auto nonspecial_ctor_with_params = [](auto r) {
        return std::meta::is_constructor(r) && !std::meta::is_special_member_function(r)
               && !std::meta::parameters_of(r).empty();
      };
      auto members = std::meta::members_of(type, std::meta::access_context::unprivileged());
      auto ctorsWithParams = members | std::views::filter(nonspecial_ctor_with_params);

      if(1 == std::ranges::distance(ctorsWithParams)) {
        return *std::ranges::begin(ctorsWithParams);
      }

      return std::meta::info{};
    }

    consteval static auto make_ctor()
    {
      return [](C const &container) consteval {
        constexpr auto ctor = find_ctor(^^T);
        if constexpr (ctor == std::meta::info{}) {
          return getter<C,T>{}(container);
        } else {
          using ctor_types = typename[:make_param_list(ctor):];
          return make(get_args(ctor_types{}, container));
        }
      };
    }

    T operator()() const
    {
      C container{};
      return operator()(container);
    }

    T operator()(C const &container) const
    {
      constexpr auto ctor = make_ctor();
      return ctor(container);
    }
  };

  template<typename... Types>
  struct builder
  {
    template<typename T>
    consteval auto add_transient()
    { return builder<Types..., T>{}; }

    consteval auto build()
    {
      struct container;

      consteval
      {
        std::vector<std::meta::info> members{};

        template for (constexpr auto t : {
                        ^^Types...,
                      })
        {
          members.push_back(data_member_spec(substitute(^^getter, { ^^container, std::meta::remove_cvref(t) }),
            { .name = std::meta::identifier_of(t) }));
        }

        define_aggregate(^^container, members);
      }

      return container{};
    }
  };

  template<>
  struct builder<>
  {
    template<typename T>
    consteval auto add_transient()
    { return builder<T>{}; }
  };
}  // namespace di