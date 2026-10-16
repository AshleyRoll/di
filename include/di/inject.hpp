#pragma once

#include <memory>

namespace di {
  namespace detail {
    // forward declare a helper to inject the value into the inject<>
    struct inject_setter;
  }

  // The inject class is used as member variables for injectable objects.
  //
  // The DI framework will ensure that registered classes that contain di::inject<>
  // members, will have those members set to a valid instance of the type before the
  // class is returned from the di provider.
  template<typename T>
  class inject
  {
    friend struct detail::inject_setter;
  public:

    auto operator->() const -> T * { return m_ptr.get(); }

  private:
    std::shared_ptr<T> m_ptr{ nullptr };
  };


}