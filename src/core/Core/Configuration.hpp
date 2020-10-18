#pragma once

#include <toml.hpp>
#include <utility>
#include <vector>

#include "Core/Command.hpp"
#include "Core/FileSystem.hpp"
#include "Core/Parameter.hpp"
#include "Core/Debug/Instrumentor.hpp"

namespace Litr {

class Configuration {
 public:
  enum class ErrorType {
    RESERVED_PARAM,
    MALFORMED_FILE,
    MALFORMED_COMMAND,
    MALFORMED_PARAM,
    MALFORMED_SCRIPT
  };

  struct Error {
    ErrorType Type;
    std::string Message;
    uint32_t Line{};
    uint32_t Column{};
    std::string LineStr{};

    Error(ErrorType type, std::string message) : Type(type), Message(std::move(message)) {
    }

    [[nodiscard]] static std::string GetTypeDescription(const Error& err) {
      LITR_PROFILE_FUNCTION();

      switch (err.Type) {
        case Configuration::ErrorType::RESERVED_PARAM:
          return "Parameter name is reserved!";
        case Configuration::ErrorType::MALFORMED_FILE:
          return "Invalid file format!";
        case Configuration::ErrorType::MALFORMED_COMMAND:
          return "Command format is wrong!";
        case Configuration::ErrorType::MALFORMED_PARAM:
          return "Parameter format is wrong!";
        case Configuration::ErrorType::MALFORMED_SCRIPT:
          return "Command script is wrong!";
      }
    }
  };

  explicit Configuration(const Path& filePath);
  virtual ~Configuration() = default;

  [[nodiscard]] std::vector<Command> GetCommands() const;
  [[nodiscard]] std::vector<Parameter> GetParameter() const;
  [[nodiscard]] std::vector<Error> GetErrors() const;

  [[nodiscard]] bool HasErrors() const;

 private:
  void CollectCommands(const toml::table& commands);
  void CollectParams(const toml::table& params);

  void AppendError(ErrorType type, const std::string& message);
  void AppendError(ErrorType type, const std::string& message, const toml::value& context);

 private:
  toml::value m_RawConfig;
  std::vector<Command> m_Commands{};
  std::vector<Parameter> m_Parameters{};

  std::vector<Error> m_Errors{};
};

}  // namespace Litr
