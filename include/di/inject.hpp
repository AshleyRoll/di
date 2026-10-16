#pragma once

#include <memory>

namespace di {

  // The inject class is used as member variables for injectable objects.
  //
  // The DI framework will ensure that registered classes that contain di::inject<>
  // members, will have those members set to a valid instance of the type before the
  // class is returned from the di provider.
  template<typename T>
  struct inject
  {
    auto operator->() const -> T * { return m_ptr.get(); }

    std::shared_ptr<T> m_ptr{ nullptr };
  };


}