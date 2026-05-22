#pragma once

#include "detail/helpers.hpp"
#include "detail/constructor.hpp"
#include "inject.hpp"

#include <meta>
#include <ranges>
#include <utility>


namespace di {
  enum struct scope {
    Transient,
    Singleton,
  };

  template<scope S, typename T>
  struct registration
  {
    constexpr static auto scope = S;
    using type = T;
  };

  template<typename T, registration... Ts>
  constexpr inline bool is_in_registration_typelist_v = (std::is_same_v<T, typename decltype(Ts)::type> || ...);

  template<registration... Ts>
  class container
  {
  private:
    // Make a struct containing a pointer to each type
    struct pointers;
    consteval
    {
      using namespace std::literals;

      constexpr auto types = std::to_array({ ^^typename decltype(Ts)::type... });
      std::vector<std::meta::info> members{};

      template for (constexpr auto i : std::views::indices(sizeof...(Ts)))
      {
        constexpr auto t = types[i];
        constexpr auto parts = std::to_array({ "m_"sv, detail::num_to_chars<i>() });
        constexpr auto name = std::string_view{ define_static_string(parts | std::views::join) };
        constexpr auto spec = data_member_spec(add_pointer(remove_cvref(t)), { .name = name });
        members.push_back(spec);
      }
      std::meta::define_aggregate(^^pointers, members);
    }
    pointers ptrs{};

    // helpful list of members
    inline static constexpr auto pointers_members =
      define_static_array(nonstatic_data_members_of(^^pointers, std::meta::access_context::current()));

  public:
    // Get an instance
    template<typename T>
    constexpr T *get()
    {
      static_assert(is_in_registration_typelist_v<std::remove_cvref_t<T>, Ts...>, "This type is not registered in the container");

      template for (constexpr auto m : pointers_members)
      {
        if constexpr (type_of(m) == add_pointer(remove_cvref(^^T))) { return ptrs.[:m:]; }
      }
      std::unreachable();
    }
  };

  template<registration... Types>
  struct fluent_builder
  {
    template<typename T>
    consteval auto add_transient()
    {
      static_assert(not is_in_registration_typelist_v<std::remove_cvref_t<T>, Types...>, "This type is already registered in the container");
      return fluent_builder<Types..., registration<scope::Transient, std::remove_cvref_t<T>>{}>{};
    }

    consteval auto build() -> container<Types...> { return {}; }
  };

  template<>
  struct fluent_builder<>
  {
    template<typename T>
    consteval auto add_transient()
    { return fluent_builder<registration<scope::Transient, std::remove_cvref_t<T>>{}>{}; }
  };

  // nice alias to start from
  using builder = fluent_builder<>;

}  // namespace di