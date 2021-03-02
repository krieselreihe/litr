#pragma once

#include <toml.hpp>
#include <vector>

#include "Core/Base.hpp"
#include "Core/Config/Command.hpp"
#include "Core/Errors/Error.hpp"
#include "Core/Errors/ErrorHandler.hpp"

namespace Litr::Config {

class CommandBuilder {
 public:
  CommandBuilder(const Ref<ErrorHandler>& errorHandler, const toml::table& file, const toml::value& data, const std::string& name);

  void AddScriptLine(const std::string& line);
  void AddScript(const std::vector<std::string>& scripts);
  void AddScript(const toml::value& scripts);

  void AddDescription();
  void AddExample();
  void AddDirectory();
  void AddOutput();
  void AddChildCommand(const Ref<Command>& command);

  [[nodiscard]] inline Ref<Command> GetResult() const { return m_Command; }

 private:
  const Ref<ErrorHandler>& m_ErrorHandler;
  const toml::table& m_File;
  const toml::value& m_Table;
  const Ref<Command> m_Command;
};

}  // namespace Litr::Config
