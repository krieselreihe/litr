#pragma once

#include <string>
#include <toml.hpp>

#include "Core/Debug/Instrumentor.hpp"

namespace Litr {

enum class ConfigurationErrorType {
  RESERVED_PARAM,
  MALFORMED_FILE,
  MALFORMED_COMMAND,
  MALFORMED_PARAM,
  MALFORMED_SCRIPT,
  UNKNOWN_COMMAND_PROPERTY
};

struct ConfigurationError {
  ConfigurationErrorType Type;
  std::string Message;
  uint32_t Line{};
  uint32_t Column{};
  std::string LineStr{};

  ConfigurationError(ConfigurationErrorType type, std::string message) : Type(type), Message(std::move(message)) {
  }

  ConfigurationError(ConfigurationErrorType type, std::string message, const toml::value& context) : Type(type), Message(std::move(message)) {
    toml::source_location location{context.location()};

    Line = location.line();
    Column = location.column();
    LineStr = location.line_str();
  }

  [[nodiscard]] static std::string GetTypeDescription(const ConfigurationError& err) {
    LITR_PROFILE_FUNCTION();

    switch (err.Type) {
      case ConfigurationErrorType::RESERVED_PARAM:
        return "Parameter name is reserved!";
      case ConfigurationErrorType::MALFORMED_FILE:
        return "Invalid file format!";
      case ConfigurationErrorType::MALFORMED_COMMAND:
        return "Command format is wrong!";
      case ConfigurationErrorType::MALFORMED_PARAM:
        return "Parameter format is wrong!";
      case ConfigurationErrorType::MALFORMED_SCRIPT:
        return "Command script is wrong!";
      case ConfigurationErrorType::UNKNOWN_COMMAND_PROPERTY:
        return "Command property does not exist!";
    }
  }
};

}  // namespace Litr
