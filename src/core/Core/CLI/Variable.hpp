#pragma once

#include <string>
#include <variant>
#include <utility>

namespace Litr::CLI {

struct Variable {
  enum class Type { STRING, BOOLEAN };

  Type Type;
  std::string Name;
  // Default value should be an empty string as booleans
  // are always handled explicit.
  std::variant<std::string, bool> Value{};

  explicit Variable(enum Type type, std::string name) : Type(type), Name(std::move(name)) {
  }
  explicit Variable(std::string name, bool value) : Type(Type::BOOLEAN), Name(std::move(name)), Value(value) {
  }
  explicit Variable(std::string name, std::string value)
      : Type(Type::STRING), Name(std::move(name)), Value(std::move(value)) {
  }
} __attribute__((aligned(64)));

}  // namespace Litr::CLI
