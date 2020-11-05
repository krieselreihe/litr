#pragma once

#include <toml.hpp>
#include <vector>

#include "Core/Base.hpp"
#include "Core/Command.hpp"
#include "Core/Errors/ConfigurationError.hpp"

namespace Litr {

class CommandBuilder {
 public:
  CommandBuilder(const toml::table& file, const toml::value& data, const std::string& name);

  void AddScriptLine(const std::string& line);
  void AddScript(const std::vector<std::string>& scripts);
  void AddScript(const toml::value& scripts);

  void AddDescription();
  void AddExample();
  void AddDirectory();
  void AddOutput();
  void AddChildCommand(const Ref<Command>& command);

  [[nodiscard]] Ref<Command> GetResult() const {
    return m_Command;
  }

  [[nodiscard]] std::vector<ConfigurationError> GetErrors() const {
      return m_Errors;
  };

 private:
  Ref<Command> m_Command;

  const toml::table& m_File;
  const toml::value& m_Table;

  std::vector<ConfigurationError> m_Errors{};
};

}  // namespace Litr
