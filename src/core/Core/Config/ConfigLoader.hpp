#pragma once

#include <deque>
#include <toml.hpp>
#include <utility>
#include <vector>

#include "Core/Base.hpp"
#include "Core/Config/Command.hpp"
#include "Core/Config/Parameter.hpp"
#include "Core/Errors/Error.hpp"
#include "Core/Errors/ErrorHandler.hpp"
#include "Core/FileSystem.hpp"

namespace Litr {

class ConfigLoader {
 public:
  explicit ConfigLoader(const Ref<ErrorHandler>& errorHandler, const Path& filePath);
  virtual ~ConfigLoader() = default;

  /**
   * Can use a dot-notation to access nested commands.
   * @todo: This should probably be moved. See implementation for details.
   */
  [[nodiscard]] Ref<Command> GetCommand(const std::string& name) const;
  [[nodiscard]] Ref<Parameter> GetParameter(const std::string& name) const;

  [[nodiscard]] inline std::vector<Ref<Command>> GetCommands() const { return m_Commands; };
  [[nodiscard]] inline std::vector<Ref<Parameter>> GetParameters() const { return m_Parameters; };

 private:
  Ref<Command> CreateCommand(const toml::table& commands, const toml::value& definition, const std::string& name);
  void CollectCommands(const toml::table& commands);
  void CollectParams(const toml::table& params);

 private:
  const Ref<ErrorHandler>& m_ErrorHandler;

  std::vector<Ref<Command>> m_Commands{};
  std::vector<Ref<Parameter>> m_Parameters{};
};

}  // namespace Litr
