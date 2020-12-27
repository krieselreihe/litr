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

  Error(ErrorType type, std::string message, const toml::value& context)
      : Type(type),
        Message(std::move(message)),
        Line(context.location().line()),
        Column(context.location().column()),
        LineStr(context.location().line_str()) {
    LITR_PROFILE_FUNCTION();
  }

  Error(ErrorType type, std::string message, const toml::exception& err)
      : Type(type),
        Message(std::move(message)),
        Line(err.location().line()),
        Column(err.location().column()),
        LineStr(err.location().line_str()) {
    LITR_PROFILE_FUNCTION();
  }
} __attribute__((aligned(64)));

}  // namespace Litr
