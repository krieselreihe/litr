#pragma once

#include <string>
#include <toml.hpp>

#include "Core/Debug/Instrumentor.hpp"

namespace Litr {

enum class ErrorType {
  RESERVED_PARAM,
  MALFORMED_FILE,
  MALFORMED_COMMAND,
  MALFORMED_PARAM,
  MALFORMED_SCRIPT,
  UNKNOWN_COMMAND_PROPERTY,
  UNKNOWN_PARAM_VALUE
};

struct Error {
  const ErrorType Type;
  const std::string Message;
  uint32_t Line{};
  uint32_t Column{};
  std::string LineStr{};

  Error(ErrorType type, std::string message) : Type(type), Message(std::move(message)) {
  }

  Error(ErrorType type, std::string message, const toml::value& context) : Type(type), Message(std::move(message)) {
    LITR_PROFILE_FUNCTION();

    const toml::source_location& location{context.location()};

    Line = location.line();
    Column = location.column();
    LineStr = location.line_str();
  }

  Error(ErrorType type, std::string message, const toml::exception& err) : Type(type), Message(std::move(message)) {
    LITR_PROFILE_FUNCTION();

    const toml::source_location& location{err.location()};

    Line = location.line();
    Column = location.column();
    LineStr = location.line_str();
  }
};

}  // namespace Litr
