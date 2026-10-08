#include "registration.hpp"

#include "di1/di.hpp"
#include "di1/container.hpp"

#include "calculator.hpp"
#include "const_1.hpp"
#include "const_2.hpp"

namespace registration {

  // only here do we need to know the full definition of all classes in the di container
  static auto di = di1::builder{}
                     .add_singleton<services::calculator>(10)  // must provide ctor params
                     .add_singleton<constants::constant_1>()
                     .add_singleton<constants::constant_2>()
                     .build();

  auto build_container() -> di1::provider { return di; }

};  // namespace registration
