#pragma once

#include <string_view>

namespace di::detail {

  // an abstract interface allowing resolution of a service (type) by name.
  struct container_interface
  {
    virtual ~container_interface() = default;

    // Returns a pointer to a std::shared_ptr<T> if found, nullptr if not.
    // Where T == the named type. T must be a class/struct
    virtual auto get_by_name(std::string_view name) -> void * = 0;
  };


}