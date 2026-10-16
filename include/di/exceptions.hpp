#pragma once

#include <format>
#include <stdexcept>
#include <string_view>

namespace di {

  class type_not_registered : public std::runtime_error
  {
  public:
    // Uses C++20 std::format for clean and safe string formatting
    explicit constexpr type_not_registered(std::string_view typeName)
      : std::runtime_error(std::format("Error: Type '{}' not found in the registry.", typeName))
    {}
  };

  class type_already_registered : public std::runtime_error
  {
  public:
    // Uses C++20 std::format for clean and safe string formatting
    explicit constexpr type_already_registered(std::string_view typeName)
      : std::runtime_error(std::format("Error: Type '{}' is already in the registry.", typeName))
    {}
  };

  class dependency_not_registered : public std::runtime_error
  {
  public:
    // Uses C++20 std::format for clean and safe string formatting
    explicit constexpr dependency_not_registered(std::string_view typeName, std::string_view dependencyName)
      : std::runtime_error(std::format("Error: Could not instantiate Type '{}', dependency '{}' not found in the registry.",
          typeName,
          dependencyName))
    {}
  };
}  // namespace di