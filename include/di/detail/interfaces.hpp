#pragma once

#include "../exceptions.hpp"
#include "helpers.hpp"

#include <memory>
#include <string_view>

namespace di {
  // The scope of an object defines when a new instance is created. An instance of earlier scope can not
  // reference an object of a later scope. That is a Singleton can only reference other Singletons, Scoped
  // can reference either a Singleton or another scoped object
  enum struct scope : std::uint8_t { Singleton, Scoped, Transient };

  namespace detail {
    class registration_interface;

    // an abstract interface allowing resolution of a service (type) by name.
    class container_interface
    {
    public:
      virtual ~container_interface() = default;

      // Returns a pointer to a std::shared_ptr<T> if found, nullptr if not.
      // Where T == the named type. T must be a class/struct
      virtual auto get_by_name(std::string_view name) -> registration_interface * = 0;

      // resolve a type and return an instance wrapped in a std::shared_ptr.
      template<typename T>
        requires std::is_class_v<std::remove_cvref_t<T>>
      auto get() -> std::shared_ptr<std::remove_cvref_t<T>>
      {
        // The container provides a type erased interface. We need to request the type my name and
        // cast the returned value back to the erased type.
        constexpr auto full_name = detail::dealiased_full_name<std::remove_cvref_t<T>>();
        auto registration = get_by_name(full_name);

        if (not registration) { throw type_not_registered{ full_name }; }

        // TODO: enforce scope dependency rules

        return std::static_pointer_cast<std::remove_cvref_t<T>>(registration->get(*this));
      }
    };

    class registration_interface
    {
    public:
      virtual ~registration_interface() = default;

      virtual auto type_name() -> std::string_view = 0;
      virtual auto scope() -> di::scope = 0;

      // returns a type erased std::shared_ptr<T> of the type registered
      virtual auto get(container_interface &container) -> std::shared_ptr<void> = 0;
    };
  }  // namespace detail
}  // namespace di