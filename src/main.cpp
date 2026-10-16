#include "calculator.hpp"
#include "registry.hpp"

#include "di/di.hpp"

#include <print>
#include <string>

struct foo
{
  explicit foo(std::string name)
    : m_name(name)
  {}

  auto message() -> std::string { return "hello " + m_name; }

  std::string m_name;
};

auto main() -> int
{
  // create a builder for our DI container
  auto builder = di::builder{};

  // can add registrations here
  builder.add_singleton<foo>("di user");  // note the ctor args can be provided

  // can pass builder to other TUs to allow for compiler firewalls
  registration::add_calculator(builder);

  // construct the live di root container
  auto di = builder.build();

  // resolve services
  auto calc = di.get<services::calculator>();
  auto f = di.get<foo>();

  // ... and use them
  std::println("Calculated: {}", calc->value());
  std::println("{}", f->message());

  return 0;
}
