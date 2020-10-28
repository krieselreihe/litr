#pragma once

#include <toml.hpp>
#include <utility>
#include <vector>

#include "Core/Command.hpp"
#include "Core/Errors.hpp"
#include "Core/FileSystem.hpp"
#include "Core/Parameter.hpp"

namespace Litr {

class Configuration {
 public:
  explicit Configuration(const Path& filePath);
  virtual ~Configuration() = default;

  [[nodiscard]] std::vector<Command> GetCommands() const;
  [[nodiscard]] std::vector<Parameter> GetParameter() const;
  [[nodiscard]] std::vector<ConfigurationError> GetErrors() const;

  [[nodiscard]] bool HasErrors() const;

 private:
  void CollectCommands(const toml::table& commands);
  void CollectParams(const toml::table& params);

  void AppendError(const ConfigurationError& error);
  void AppendError(ConfigurationErrorType type, const std::string& message);
  void AppendError(ConfigurationErrorType type, const std::string& message, const toml::value& context);

 private:
  toml::value m_RawConfig;
  std::vector<Command> m_Commands{};
  std::vector<Parameter> m_Parameters{};

  std::vector<ConfigurationError> m_Errors{};
};

}  // namespace Litr
