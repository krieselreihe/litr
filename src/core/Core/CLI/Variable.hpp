#pragma once

#include <string>
#include <variant>
#include <utility>

#include "Core/Config/Parameter.hpp"

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
  explicit Variable(const Config::Parameter& parameter) {
    Name = parameter.Name;

    switch (parameter.Type) {
      case Config::Parameter::Type::STRING:
      case Config::Parameter::Type::ARRAY:
        Type = Type::STRING;
        Value = "";
        break;
      case Config::Parameter::Type::BOOLEAN:
        Type = Type::BOOLEAN;
        Value = false;
        break;
    }
  }
};

}  // namespace Litr::CLI
