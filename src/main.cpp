#include "di/di.hpp"

#include "registration.hpp"
#include "calculator.hpp"

#include <iostream>

auto main() -> int
{
  auto di = registration::build_container();

  auto calc = di.get<services::calculator>();

  std::cout << "Calculated: " << calc->value() << "\n";

  return 0;
}
