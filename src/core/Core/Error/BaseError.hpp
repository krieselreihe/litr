#pragma once

#include <string>
#include <toml.hpp>
#include <utility>

#include "Core/Debug/Instrumentor.hpp"

namespace Litr::Error {

enum class ErrorType {
  RESERVED_PARAM,            // Reserved configuration parameter name
  MALFORMED_FILE,            // Configuration file malformed
  MALFORMED_COMMAND,         // Command configuration malformed
  MALFORMED_PARAM,           // Parameter configuration malformed
  MALFORMED_SCRIPT,          // Configuration command script malformed
  UNKNOWN_COMMAND_PROPERTY,  // Unknown option used for command in configuration
  UNKNOWN_PARAM_VALUE,       // Unknown option used for parameter in configuration
  PARSER_ERROR,              // Error while parsing CLI input arguments
  COMMAND_NOT_FOUND,         // On execution, command not found
  EXECUTION_FAILURE          // Issue executing a command
};

struct BaseError {
  const ErrorType Type;
  const std::string Message;
  uint32_t Line{};
  uint32_t Column{};
  std::string LineStr{};

  BaseError(ErrorType type, std::string message) : Type(type), Message(std::move(message)) {
    LITR_PROFILE_FUNCTION();
  }

  BaseError(ErrorType type, std::string message, uint32_t line, uint32_t column, std::string lineStr)
      : Type(type), Message(std::move(message)), Line(line), Column(column), LineStr(std::move(lineStr)) {
    LITR_PROFILE_FUNCTION();
  }

  BaseError(ErrorType type, std::string message, const toml::value& context)
      : Type(type),
        Message(std::move(message)),
        Line(context.location().line()),
        Column(context.location().column()),
        LineStr(context.location().line_str()) {
    LITR_PROFILE_FUNCTION();
  }

  BaseError(ErrorType type, std::string message, const toml::exception& err)
      : Type(type),
        Message(std::move(message)),
        Line(err.location().line()),
        Column(err.location().column()),
        LineStr(err.location().line_str()) {
    LITR_PROFILE_FUNCTION();
  }
} __attribute__((aligned(64)));

}  // namespace Litr::Error
