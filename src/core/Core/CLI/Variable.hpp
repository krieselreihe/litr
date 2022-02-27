/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

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

  Variable(enum Type type, std::string name) : Type(type), Name(std::move(name)) {
  }
  Variable(std::string name, bool value) : Type(Type::BOOLEAN), Name(std::move(name)), Value(value) {
  }
  Variable(std::string name, std::string value)
      : Type(Type::STRING), Name(std::move(name)), Value(std::move(value)) {
  }
  explicit Variable(const Config::Parameter& parameter) : Name(parameter.Name) {
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
