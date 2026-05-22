#include "di/di.hpp"

#include <iostream>

class dependency_1
{
public:
  dependency_1() = default;
  auto value() const -> int { return m_value; }

private:
  int m_value{ 100 };
};

namespace test {
  class dependency_1  // repeated name in different ns
  {
  public:
    dependency_1() = default;
    auto value() const -> int { return m_value; }

  private:
    int m_value{ 69 };
  };


  class service
  {
  public:
    explicit service(int multiplier)
      : m_multiplier(multiplier)
    {}

    auto value() const -> int { return m_multiplier * m_dependency->value(); }

  private:
    int m_multiplier;
    di::inject<dependency_1> m_dependency;
  };
}  // namespace test

namespace di {}  // namespace di

int main()
{

  auto di = di::builder{}
    .add_transient<test::service>()
    .add_transient<test::dependency_1>()
    .add_transient<dependency_1>()
    .build();

  template for(constexpr auto m : define_static_array(nonstatic_data_members_of(^^test::service, std::meta::access_context::unchecked()))) {
    constexpr auto t = type_of(m);
    std::cout << identifier_of(m) << " " << display_string_of(t) << " " << has_template_arguments(t) << std::endl;
    if constexpr (has_template_arguments(t)) {
      constexpr auto inject_type = ^^di::inject;

      std::cout << "  -> " << display_string_of(template_of(t)) << " " << std::endl;
      if constexpr (inject_type == template_of(t)) {
        static constexpr auto params = define_static_array(template_arguments_of(t));
        template for(constexpr auto p : params) {
          std::cout << "    -> " << display_string_of(p) << " " << std::endl;
        }
      }

    }
  }


  return di.get<test::dependency_1>() == nullptr;
}
