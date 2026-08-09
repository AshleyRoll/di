#pragma once

namespace di {

  // The inject class is used as member variables for injectable objects.
  // the object must be created from a container or container scope, and it will
  // fill these objects before returning the instance.
  template<typename T>
  struct inject
  {
    auto operator->() const -> T * { return m_ptr; }

    T *m_ptr{ nullptr };
  };


}