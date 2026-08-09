#pragma once

#include "detail/helpers.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace di {

  // an abstract interface allowing resolution of a service (type) by name.
  struct container_base
  {
    virtual ~container_base() = default;
    virtual auto get_by_name(std::string_view name) -> void * = 0;
  };

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
    explicit constexpr provider(std::shared_ptr<container_base> provider)
      : m_provider(std::move(provider))
    {}

    template<typename T>
    constexpr auto get() -> T *
    {
      constexpr auto full_name = detail::dealiased_full_name<T>();
      auto ptr = static_cast<T *>(m_provider->get_by_name(full_name));
      if (!ptr) { throw std::runtime_error{ std::string{"Type not registered: "} + full_name }; }
      return ptr;
    }

  private:
    std::shared_ptr<container_base> m_provider;
  };
}  // namespace di