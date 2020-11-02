#pragma once

#include <toml.hpp>
#include <vector>

#include "Core/Command.hpp"
#include "Core/Errors/ConfigurationError.hpp"

namespace Litr {

class CommandBuilder {
 public:
  CommandBuilder(const std::string& name, const toml::value& data, const toml::table& file);

  void AddScriptLine(const std::string& line);
  void AddScript(const std::vector<std::string>& scripts);
  void AddScript(const toml::value& scripts);

  void AddDescription();
  void AddExample();
  void AddDirectory();
  void AddOutput();

  [[nodiscard]] Command GetResult() const {
    return m_Command;
  }

  [[nodiscard]] std::vector<ConfigurationError> GetErrors() const {
      return m_Errors;
  };

 private:
  Command m_Command;
  const toml::value& m_Table;
  const toml::table& m_File;
  std::vector<ConfigurationError> m_Errors{};
};

}  // namespace Litr
