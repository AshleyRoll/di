#pragma once

#include "detail/container_interface.hpp"
#include "detail/helpers.hpp"
#include "exceptions.hpp"

#include <memory>
#include <string_view>
#include <type_traits>
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
    explicit constexpr provider(std::shared_ptr<detail::container_interface> container)
      : m_container(std::move(container))
    {}

    // resolve a type and return an instance wrapped in a std::shared_ptr.
    template<typename T>
      requires std::is_class_v<std::remove_cvref_t<T>>
    constexpr auto get() -> std::shared_ptr<std::remove_cvref_t<T>>
    {
      // The container provides a type erased interface. We need to request the type my name and
      // cast the returned value back to the erased type.
      constexpr auto full_name = detail::dealiased_full_name<std::remove_cvref_t<T>>();
      auto ptr = static_cast<std::shared_ptr<T> *>(m_container->get_by_name(full_name));

      if (!ptr) { throw type_not_registered{ full_name }; }

      return *ptr;
    }

  private:
    std::shared_ptr<detail::container_interface> m_container;
  };


}  // namespace di