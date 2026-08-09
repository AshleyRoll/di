#include "registration.hpp"

#include "di/di.hpp"
#include "di/container.hpp"

#include "calculator.hpp"
#include "const_1.hpp"
#include "const_2.hpp"

namespace registration {

  // only here do we need to know the full definition of all classes in the di container
  static auto di = di::builder{}
                     .add_singleton<services::calculator>(10)  // must provide ctor params
                     .add_singleton<constants::constant_1>()
                     .add_singleton<constants::constant_2>()
                     .build();

  auto build_container() -> di::provider { return di; }

};  // namespace registration
