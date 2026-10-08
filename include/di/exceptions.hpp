#pragma once

#include <format>
#include <stdexcept>
#include <string_view>

namespace di {

  class type_not_registered : public std::runtime_error
  {
  public:
    // Uses C++20 std::format for clean and safe string formatting
    explicit type_not_registered(std::string_view type_name)
      : std::runtime_error(std::format("Error: Type {} not found in the registry.", type_name))
    {}
  };

  class type_already_registered : public std::runtime_error
  {
  public:
    // Uses C++20 std::format for clean and safe string formatting
    explicit type_already_registered(std::string_view type_name)
      : std::runtime_error(std::format("Error: Type {} is already in the registry.", type_name))
    {}
  };

  class dependency_not_registered : public std::runtime_error
  {
  public:
    // Uses C++20 std::format for clean and safe string formatting
    explicit dependency_not_registered(std::string_view type_name, std::string_view dependency_name)
      : std::runtime_error(std::format("Error: Could not instantiate Type {}, dependency {} not found in the registry.",
          type_name,
          dependency_name))
    {}
  };
}  // namespace di