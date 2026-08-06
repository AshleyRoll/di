#include "di/di.hpp"
#include "di/inject.hpp"

#include <iostream>

struct constant_100
{
  auto value() const -> int { return 100; }
};

struct constant_69
{
  auto value() const -> int { return 69; }
};

class calculator
{
public:
  calculator(int multiplier)  // non-injected ctor params
    : m_multiplier(multiplier)
  {}

  auto value() const -> int { return m_multiplier * (m_dep_100->value() + m_dep_69->value()); }

private:
  int m_multiplier;
  // automatically injected instances or doom
  di::inject<constant_69> m_dep_69;
  di::inject<constant_100> m_dep_100;
};

namespace di {}  // namespace di

auto main() -> int
{
  auto di = di::builder{}
              .add_singleton<calculator>(10)  // must provide ctor params
              .add_singleton<constant_69>()
              .add_singleton<constant_100>()
              .build();

  auto c = di.get<calculator>();

  std::cout << "Calculated: " << c->value() << "\n";


  return 0;
}
