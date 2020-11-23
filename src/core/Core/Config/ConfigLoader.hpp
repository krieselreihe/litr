#pragma once

#include <toml.hpp>
#include <utility>
#include <vector>
#include <deque>

#include "Core/Base.hpp"
#include "Core/Config/Command.hpp"
#include "Core/Config/Parameter.hpp"
#include "Core/Errors/ConfigurationError.hpp"
#include "Core/FileSystem.hpp"

namespace Litr {

class ConfigLoader {
 public:
  explicit ConfigLoader(const Path& filePath);
  virtual ~ConfigLoader() = default;

  /**
   * Can use a dot-notation to access nested commands.
   * @todo: This should probably be moved. See implementation for details.
   */
  [[nodiscard]] Ref<Command> GetCommand(const std::string& name) const;
  [[nodiscard]] Ref<Parameter> GetParameter(const std::string& name) const;

  [[nodiscard]] inline std::vector<Ref<Command>> GetCommands() const { return m_Commands; };
  [[nodiscard]] inline std::vector<Ref<Parameter>> GetParameters() const { return m_Parameters; };
  [[nodiscard]] inline std::vector<ConfigurationError> GetErrors() const { return m_Errors; };
  [[nodiscard]] inline bool HasErrors() const { return !m_Errors.empty(); };

 private:
  Ref<Command> CreateCommand(const toml::table& commands, const toml::value& definition, const std::string& name);
  void CollectCommands(const toml::table& commands);
  void CollectParams(const toml::table& params);

  void AppendErrors(const std::vector<ConfigurationError>& errors);

 private:
  std::vector<Ref<Command>> m_Commands{};
  std::vector<Ref<Parameter>> m_Parameters{};
  std::vector<ConfigurationError> m_Errors{};
};

}  // namespace Litr
