/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>
#include <variant>
#include <utility>

#include "Core/Config/Parameter.hpp"

namespace litr::cli {

struct Variable {
  enum class Type { STRING, BOOLEAN };

  Type type;
  std::string name;
  // Default value should be an empty string as booleans
  // are always handled explicit.
  std::variant<std::string, bool> value{};

  Variable(enum Type type, std::string name) : type(type), name(std::move(name)) {
  }
  Variable(std::string name, bool value) : type(Type::BOOLEAN), name(std::move(name)), value(value) {
  }
  Variable(std::string name, std::string value)
      : type(Type::STRING), name(std::move(name)), value(std::move(value)) {
  }
  explicit Variable(const config::Parameter& parameter) : name(parameter.name) {
    switch (parameter.type) {
      case config::Parameter::Type::STRING:
      case config::Parameter::Type::ARRAY:
        type = Type::STRING;
        value = "";
        break;
      case config::Parameter::Type::BOOLEAN:
        type = Type::BOOLEAN;
        value = false;
        break;
    }
  }
};

}  // namespace litr::cli
