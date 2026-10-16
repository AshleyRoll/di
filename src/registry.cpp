#include "registry.hpp"

#include "di/di.hpp"

#include "calculator.hpp"
#include "const_1.hpp"
#include "const_2.hpp"

namespace registration {

  void add_calculator(di::builder &builder)
  {
    builder
      .add_transient<services::calculator>(10)  // must provide ctor params
      .add_singleton<constants::constant_1>()
      .add_singleton<constants::constant_2>();
  }
};  // namespace registration
