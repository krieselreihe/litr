#pragma once

#include <toml.hpp>
#include <utility>
#include <vector>

#include "Core/Base.hpp"
#include "Core/Command.hpp"
#include "Core/Errors/ConfigurationError.hpp"
#include "Core/FileSystem.hpp"
#include "Core/Parameter.hpp"

namespace Litr {

class Configuration {
 public:
  explicit Configuration(const Path& filePath);
  virtual ~Configuration() = default;

  [[nodiscard]] inline std::vector<Ref<Command>> GetCommands() const { return m_Commands; };
  [[nodiscard]] inline std::vector<Parameter> GetParameter() const { return m_Parameters; };
  [[nodiscard]] inline std::vector<ConfigurationError> GetErrors() const { return m_Errors; };
  [[nodiscard]] inline bool HasErrors() const { return !m_Errors.empty(); };

 private:
  Ref<Command> CreateCommand(const toml::table& commands, const toml::value& definition, const std::string& name);
  void CollectCommands(const toml::table& commands);
  void CollectParams(const toml::table& params);

 private:
  toml::value m_RawConfig;
  std::vector<Ref<Command>> m_Commands{};
  std::vector<Parameter> m_Parameters{};

  std::vector<ConfigurationError> m_Errors{};
};

}  // namespace Litr
