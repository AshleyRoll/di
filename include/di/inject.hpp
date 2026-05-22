#pragma once

namespace di {

  // The inject class is used as member variables for injectable objects.
  // the object must be created from a container or container scope, and it will
  // fill these objects before returning the instance.
  template<typename T>
  class inject
  {
  public:
    inject() = default;
    ~inject()
    {
      if (m_deleter) { m_deleter(m_ptr); }
      m_ptr = nullptr;
      m_deleter = nullptr;
    }

    T *operator->() const { return m_ptr; }

  private:
    T *m_ptr{ nullptr };
    void (*m_deleter)(T *){ nullptr };
  };


}