/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>
#include <utility>

#include <toml.hpp>

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Config/Location.hpp"
#include "Core/Error/TomlError.hpp"

namespace litr::Error {

// Forward declaration for `friend` declaration of litr::Error::Reporter.
class Reporter;

class BaseError {
 protected:
  enum class ErrorType {
    RESERVED_PARAM,            // Reserved configuration parameter name
    MALFORMED_FILE,            // Configuration file malformed
    MALFORMED_COMMAND,         // Command configuration malformed
    MALFORMED_PARAM,           // Parameter configuration malformed
    MALFORMED_SCRIPT,          // Configuration command script malformed
    UNKNOWN_COMMAND_PROPERTY,  // Unknown option used for command in configuration
    UNKNOWN_PARAM_VALUE,       // Unknown option used for parameter in configuration
    VALUE_ALREADY_IN_USE,      // A value is already used, e.g. a Shortcut name
    CLI_PARSER,                // Error while parsing CLI input arguments
    SCRIPT_PARSER,             // Error while parsing scripts
    COMMAND_NOT_FOUND,         // On execution, command not found
    EXECUTION_FAILURE          // Issue executing a command
  };

  BaseError(ErrorType type, std::string message) : Type(type), Message(std::move(message)) {
    LITR_PROFILE_FUNCTION();
  }

  BaseError(ErrorType type, std::string message, uint32_t line, uint32_t column, std::string lineStr)
      : Type(type), Message(std::move(message)), Location(line, column, std::move(lineStr)) {
    LITR_PROFILE_FUNCTION();
  }

  BaseError(ErrorType type, std::string message, const toml::value& context)
      : Type(type),
        Message(std::move(message)),
        Location(context.location().line(), context.location().column(), context.location().line_str()) {
    LITR_PROFILE_FUNCTION();
  }

  BaseError(ErrorType type, std::string message, const toml::exception& err)
      : Type(type),
        Message(std::move(message)),
        Location(err.location().line(), err.location().column(), err.location().line_str()) {
    LITR_PROFILE_FUNCTION();
  }

 public:
  friend Reporter;

  const ErrorType Type;
  const std::string Message;

  std::string Description{};
  Config::Location Location{};
};

class ReservedParamError : public BaseError {
 public:
  ReservedParamError(const std::string& message, const toml::value& context)
      : BaseError(ErrorType::RESERVED_PARAM, message, context) {
    BaseError::Description = "Parameter name is reserved!";
  }
};

class MalformedFileError : public BaseError {
 public:
  explicit MalformedFileError(const std::string& message)
      : BaseError(ErrorType::MALFORMED_FILE, message) {
  }
  MalformedFileError(const std::string& message, const toml::exception& err)
      : BaseError(ErrorType::MALFORMED_FILE, TomlError::ExtractMessage(message, err.what()), err) {
    BaseError::Description = "Invalid file format!";
  }
};

class MalformedCommandError : public BaseError {
 public:
  MalformedCommandError(const std::string& message, const toml::value& context)
      : BaseError(ErrorType::MALFORMED_COMMAND, message, context) {
    BaseError::Description = "Command format is wrong!";
  }
};

class MalformedParamError : public BaseError {
 public:
  MalformedParamError(const std::string& message, const toml::value& context)
      : BaseError(ErrorType::MALFORMED_PARAM, message, context) {
    BaseError::Description = "Parameter format is wrong!";
  }
};

class MalformedScriptError : public BaseError {
 public:
  MalformedScriptError(const std::string& message, const toml::value& context)
      : BaseError(ErrorType::MALFORMED_SCRIPT, message, context) {
    BaseError::Description = "Command script is wrong!";
  }
};

class UnknownCommandPropertyError : public BaseError {
 public:
  UnknownCommandPropertyError(const std::string& message, const toml::value& context)
      : BaseError(ErrorType::UNKNOWN_COMMAND_PROPERTY, message, context) {
    BaseError::Description = "Command property does not exist!";
  }
};

class UnknownParamValueError : public BaseError {
 public:
  UnknownParamValueError(const std::string& message, const toml::value& context)
      : BaseError(ErrorType::UNKNOWN_PARAM_VALUE, message, context) {
    BaseError::Description = "Parameter value is not known!";
  }
};

class ValueAlreadyInUseError : public BaseError {
 public:
  ValueAlreadyInUseError(const std::string& message, const toml::value& context)
      : BaseError(ErrorType::VALUE_ALREADY_IN_USE, message, context) {
    BaseError::Description = "Value is is already in use!";
  }
};

class CLIParserError : public BaseError {
 public:
  CLIParserError(const std::string& message, uint32_t line, uint32_t column, const std::string& lineStr)
      : BaseError(ErrorType::CLI_PARSER, message, line, column, lineStr) {
    BaseError::Description = "Problem parsing command line arguments!";
  }
};

class ScriptParserError : public BaseError {
 public:
  ScriptParserError(const std::string& message, uint32_t line, uint32_t column, const std::string& lineStr)
      : BaseError(ErrorType::CLI_PARSER, message, line, column, lineStr) {
    BaseError::Description = "Problem parsing script!";
  }
};

class CommandNotFoundError : public BaseError {
 public:
  explicit CommandNotFoundError(const std::string& message)
      : BaseError(ErrorType::COMMAND_NOT_FOUND, message) {
    BaseError::Description = "Command not found!";
  }
};

class ExecutionFailureError : public BaseError {
 public:
  explicit ExecutionFailureError(const std::string& message)
      : BaseError(ErrorType::EXECUTION_FAILURE, message) {
    BaseError::Description = "Problem executing command!";
  }
};

}  // namespace litr::Error
