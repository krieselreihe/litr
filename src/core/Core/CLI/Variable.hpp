/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>
#include <utility>
#include <variant>

#include "Core/Config/Parameter.hpp"

namespace Litr::CLI {

using namespace std::string_literals;

struct Variable {
  enum class Type { STRING, BOOLEAN };

  Type type;
  std::string name;
  // Default value should be an empty string as booleans
  // are always handled explicit.
  std::variant<std::string, bool> value{""s};

  Variable(enum Type type, std::string name) : type(type), name(std::move(name)) {}

  Variable(std::string name, const bool value)
      : type(Type::BOOLEAN),
        name(std::move(name)),
        value(value) {}

  Variable(std::string name, std::string value)
      : type(Type::STRING),
        name(std::move(name)),
        value(std::move(value)) {}

  explicit Variable(const Config::Parameter& parameter) : name(parameter.name) {
    switch (parameter.type) {
      case Config::Parameter::Type::STRING:
      case Config::Parameter::Type::ARRAY:
        type = Type::STRING;
        value = ""s;
        break;
      case Config::Parameter::Type::BOOLEAN:
        type = Type::BOOLEAN;
        value = false;
        break;
    }
  }
};

}  // namespace Litr::CLI
