#pragma once

#include "di/inject.hpp"

// forward declare our services, or we could include their definitions
// if we move our implementation of the code using the injected objects, they can be forward declared
namespace constants {
  struct constant_1;
  struct constant_2;
}

namespace services {

  class calculator
  {
  public:
    calculator(int multiplier)  // non-injected ctor params
      : m_multiplier(multiplier)
    {}

    auto value() const -> int;

  private:
    int m_multiplier;
    // automatically injected instances or doom
    di::inject<constants::constant_1> m_c1;
    di::inject<constants::constant_2> m_c2;
  };
}