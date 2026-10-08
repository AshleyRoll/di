#include "di/provider.hpp"

#include "calculator.hpp"
#include "registration.hpp"

#include <print>

auto main() -> int
{
  auto di = registration::build_container();

  auto calc = di.get<services::calculator>();

  std::println("Calculated: {}", calc->value());

  return 0;
}
