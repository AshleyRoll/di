#pragma once

#include "detail/constructor.hpp"
#include "detail/helpers.hpp"

#include <cstdint>
#include <meta>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>


namespace di {
  enum struct scope : std::uint8_t {
    Transient,  // TODO implement transient scope
    Singleton,
  };

  struct no_args_tag
  {
  };

  template<scope S, typename T, typename... Args>
  struct registration
  {
    constexpr static auto scope = S;
    using type = std::remove_cvref_t<T>;

    using ctor_args_type =
      std::conditional_t<sizeof...(Args) == 0, std::tuple<no_args_tag>, std::tuple<std::remove_cvref_t<Args>...>>;
  };


  template<typename T, registration... Ts>
  constexpr inline bool is_in_registration_typelist_v =
    (std::is_same_v<std::remove_cvref_t<T>, typename decltype(Ts)::type> || ...);

  template<registration... Ts>
  class container
  {
  private:
    // the tuple of tuples of construction args
    using ArgsTuples = std::tuple<typename decltype(Ts)::ctor_args_type...>;
    ArgsTuples ctor_args;

    // helpful list of members
    static constexpr auto registration_types =
      std::define_static_array(std::to_array({ ^^typename decltype(Ts)::type... }));

    // Make a struct containing a pointer to each type
    struct pointers;
    consteval
    {
      using namespace std::literals;

      // constexpr auto types = std::to_array({ ^^typename decltype(Ts)::type... });
      std::vector<std::meta::info> members{};

      template for (constexpr auto i : std::views::indices(sizeof...(Ts)))
      {
        constexpr auto t = registration_types[i];
        constexpr auto parts = std::to_array({ "m_"sv, detail::num_to_chars<i>() });
        constexpr auto name = std::string_view{ define_static_string(parts | std::views::join) };
        constexpr auto spec = data_member_spec(^^std::unique_ptr<typename[:remove_cvref(t):]>, { .name = name });
        members.push_back(spec);
      }
      std::meta::define_aggregate(^^pointers, members);
    }
    pointers ptrs{};


    template<typename T>
    consteval static auto get_index_of_registration_type() -> std::size_t
    {
      for (std::size_t const i : std::views::indices(sizeof...(Ts))) {
        auto t = registration_types[i];
        if (is_same_type(t, ^^std::remove_cvref_t<T>)) { return i; }
      }
      std::unreachable();
    }

    // helpful list of members
    static constexpr auto pointers_members =
      std::define_static_array(std::meta::nonstatic_data_members_of(^^pointers, std::meta::access_context::current()));

  public:
    explicit constexpr container(ArgsTuples &ctor_args)
      : ctor_args{ ctor_args }
    {}

    // Get an instance
    template<typename T>
    constexpr auto get() -> T *
    {
      static_assert(is_in_registration_typelist_v<T, Ts...>, "This type is not registered in the container");

      constexpr auto type_index = get_index_of_registration_type<T>();

      template for (constexpr auto m : pointers_members)
      {
        if constexpr (is_same_type(type_of(m), ^^std::unique_ptr<typename[:remove_cvref(^^T):]>)) {
          constexpr auto ctor_type = ^^typename decltype(Ts...[type_index])::ctor_args_type;

          if (ptrs.[:m:] == nullptr) {
            if constexpr (is_same_type(ctor_type, ^^std::tuple<no_args_tag>)) {
              // no args
              ptrs.[:m:] = detail::constructor<T, container>::make(this);
            } else {
              auto params = std::tuple_cat(std::make_tuple(this), std::get<type_index>(ctor_args));
              ptrs.[:m:] = std::apply(
                           [](auto &&...args) {
                             return detail::constructor<T, container>::make(std::forward<decltype(args)>(args)...);
                           },
                           params);
            }
          }

          return ptrs.[:m:].get();
        }
      }
      std::unreachable();
    }
  };

  template<registration... Types>
  struct fluent_builder
  {
    using ArgsTuples = std::tuple<typename decltype(Types)::ctor_args_type...>;
    ArgsTuples ctor_args;

    template<typename T, typename... Args>
    constexpr auto add_singleton(Args &&...args)
    {
      static_assert(not is_in_registration_typelist_v<T, Types...>, "This type is already registered in the container");

      if constexpr (sizeof...(Args) == 0) {
        return fluent_builder<Types..., registration<scope::Singleton, T, Args...>{}>{
          std::tuple_cat(ctor_args, std::make_tuple(no_args_tag{})),
        };
      } else {
        return fluent_builder<Types..., registration<scope::Singleton, T, Args...>{}>{
          std::tuple_cat(ctor_args, std::make_tuple(std::forward<Args>(args)...)),
        };
      }
    }

    auto build() -> container<Types...> { return container<Types...>{ ctor_args }; }
  };

  template<>
  struct fluent_builder<>
  {
    template<typename T, typename... Args>
    constexpr auto add_singleton(Args &&...args)
    {
      return fluent_builder<registration<scope::Singleton, T, Args...>{}>{ std::make_tuple(
        std::forward<Args>(args)...) };
    }
  };

  // nice alias to start from
  using builder = fluent_builder<>;

}  // namespace di