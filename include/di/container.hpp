#pragma once

#include "detail/constructor.hpp"
#include "detail/helpers.hpp"
#include "di.hpp"

#include <cstdint>
#include <meta>
#include <ranges>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>


namespace di {
  enum struct scope : std::uint8_t {
    Transient,  // TODO implement transient scope
    Singleton,
  };

  namespace detail {
    struct no_args_tag
    {
    };

    // used in the args lists to indicate no arguments are needed for this type constructor
    using no_args = std::tuple<no_args_tag>;
  }  // namespace detail

  template<scope S, typename T, typename... Args>
  struct registration
  {
    constexpr static auto Scope = S;
    using type = std::remove_cvref_t<T>;

    using ctor_args_type =
      std::conditional_t<sizeof...(Args) == 0, detail::no_args, std::tuple<std::remove_cvref_t<Args>...>>;
  };


  template<typename T, registration... Ts>
  constexpr inline bool is_in_registration_typelist_v =
    (std::is_same_v<std::remove_cvref_t<T>, typename decltype(Ts)::type> || ...);

  template<registration... Ts>
  class container final : public container_base
  {
    constexpr static std::size_t NumObjects = sizeof...(Ts);

    // the tuple of tuples of construction args
    using args_tuples = std::tuple<typename decltype(Ts)::ctor_args_type...>;

  public:
    explicit constexpr container(args_tuples &ctor_args)
      : m_ctorArgs{ ctor_args }
    {}

    // Get an instance, possibly building other instances needed in creating this one
    template<typename T>
    constexpr auto get() -> T *
    {
      static_assert(is_in_registration_typelist_v<T, Ts...>, "This type is not registered in the container");

      constexpr auto type_index = get_index_of_registration_type<T>();

      constexpr auto m = pointers_members[type_index];
      if constexpr (is_same_type(type_of(m), ^^std::unique_ptr<typename[:remove_cvref(^^T):]>)) {
        constexpr auto ctor_type = ^^typename decltype(Ts...[type_index])::ctor_args_type;

        if (ptrs.[:m:] == nullptr) {
          if constexpr (is_same_type(ctor_type, ^^detail::no_args)) {
            // no args
            ptrs.[:m:] = detail::constructor<T, container>::make(this);
          } else {
            // there are arguments in the ctor_args array for this indexed type
            auto params = std::tuple_cat(std::make_tuple(this), std::get<type_index>(m_ctorArgs));
            ptrs.[:m:] = std::apply(
                         []<typename... Args>(Args &&...args) {
                           return detail::constructor<T, container>::make(std::forward<Args>(args)...);
                         },
                         params);
          }
        }

        return ptrs.[:m:].get();
      }
      std::unreachable();
    }

    // use the pre-computed name->getter type erasure map
    //
    // This is used by the di::provider to implement the type erased interface to the container
    auto get_by_name(std::string_view name) -> void * override
    {
      if (getter_map.contains(name)) { return getter_map[name](*this); }
      return nullptr;
    }


  private:
    // the tuple of tuples of construction args
    args_tuples m_ctorArgs;

    // helpful list of members
    static constexpr auto registration_types =
      std::define_static_array(std::to_array({ ^^typename decltype(Ts)::type... }));

    // a generic getter function type to retrieve an instance
    using getter_func = void *(*)(container &);

    // build the map use to lookup a getter by type name. This is used to implement the
    // type erased di::provider interface
    std::unordered_map<std::string_view, getter_func> getter_map = [] -> auto {
      std::unordered_map<std::string_view, getter_func> map{};

      template for (constexpr auto i : std::views::indices(sizeof...(Ts)))
      {
        constexpr auto t = registration_types[i];
        constexpr auto name = detail::dealiased_full_name(t);
        map.emplace(name, [](container &self) -> void * { return self.get<typename[:t:]>(); });
      }

      return map;
    }();

    // Make a struct containing a unique_ptr to each type to manage lifetime
    struct pointers;
    consteval
    {
      using namespace std::literals;

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

    // helper to lookup the index of a registered type into the array/ptrs struct
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
  };

  template<registration... Types>
  struct fluent_builder
  {
    using args_tuples = std::tuple<typename decltype(Types)::ctor_args_type...>;
    args_tuples ctor_args;

    // Register a singleton type
    template<typename T, typename... Args>
    constexpr auto add_singleton(Args &&...args)
    {
      static_assert(std::is_class_v<T>, "Must be a class or struct");
      static_assert(std::is_constructible_v<T, Args...>, "No matching constructor for the provided arguments");
      static_assert(not is_in_registration_typelist_v<T, Types...>, "This type is already registered in the container");

      if constexpr (sizeof...(Args) == 0) {
        return fluent_builder<Types..., registration<scope::Singleton, T, Args...>{}>{
          std::tuple_cat(ctor_args, detail::no_args{}),
        };
      } else {
        return fluent_builder<Types..., registration<scope::Singleton, T, Args...>{}>{
          std::tuple_cat(ctor_args, std::make_tuple(std::forward<Args>(args)...)),
        };
      }
    }

    auto build() -> provider { return provider{ std::make_shared<container<Types...>>(ctor_args) }; }
  };

  template<>
  struct fluent_builder<>
  {
    // register a singleton type
    template<typename T, typename... Args>
    constexpr auto add_singleton(Args &&...args)
    {
      static_assert(std::is_class_v<T>, "Must be a class or struct");
      static_assert(std::is_constructible_v<T, Args...>, "No matching constructor for the provided arguments");

      return fluent_builder<registration<scope::Singleton, T, Args...>{}>{ std::make_tuple(
        std::forward<Args>(args)...) };
    }
  };

  // nice alias to start from
  using builder = fluent_builder<>;

}  // namespace di