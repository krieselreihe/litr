#include "CommandBuilder.hpp"

#include <fmt/format.h>

namespace Litr {

CommandBuilder::CommandBuilder(const toml::table& file, const toml::value& data, const std::string& name) : m_File(file), m_Table(data) {
  m_Command = CreateRef<Command>(name);
  LITR_CORE_TRACE("Creating {}", *m_Command);
}

void CommandBuilder::AddScriptLine(const std::string& line) {
  m_Command->Script.emplace_back(line);
}

void CommandBuilder::AddScript(const std::vector<std::string>& scripts) {
  m_Command->Script = scripts;
}

void CommandBuilder::AddScript(const toml::value& scripts) {
  for (const auto& script : scripts.as_array()) {
    if (!script.is_string()) {
      m_Errors.emplace_back(
          ConfigurationErrorType::MALFORMED_SCRIPT,
          "A command script can be either a string or array of strings.",
          m_File.at(m_Command->Name));
      // Stop after first error in an array of scripts, to avoid being to verbose.
      break;
    }

    AddScriptLine(script.as_string());
  }
}

void CommandBuilder::AddDescription() {
  std::string name{"description"};

  if (m_Table.contains(name)) {
    toml::value description{toml::find(m_Table, name)};
    if (description.is_string()) {
      m_Command->Description = description.as_string();
    } else {
      m_Errors.emplace_back(
          ConfigurationErrorType::MALFORMED_COMMAND,
          fmt::format(R"(The "{}" can can only be a string.)", name),
          m_Table.at(name));
    }
  }
}

void CommandBuilder::AddExample() {
  std::string name{"example"};

  if (m_Table.contains(name)) {
    toml::value example{toml::find(m_Table, name)};
    if (example.is_string()) {
      m_Command->Example = example.as_string();
    } else {
      m_Errors.emplace_back(
          ConfigurationErrorType::MALFORMED_COMMAND,
          fmt::format(R"(The "{}" can can only be a string.)", name),
          m_Table.at(name));
    }
  }
}

void CommandBuilder::AddDirectory() {
  std::string name{"dir"};

  if (m_Table.contains(name)) {
    toml::value dir{toml::find(m_Table, name)};
    if (dir.is_string()) {
      m_Command->Directory.emplace_back(dir.as_string());
    } else if (dir.is_array()) {
      for (auto d : dir.as_array()) {
        m_Command->Directory.emplace_back(d.as_string());
      }
    } else {
      m_Errors.emplace_back(
          ConfigurationErrorType::MALFORMED_COMMAND,
          fmt::format(R"(A "{}" can either be a string or array of strings.)", name),
          m_Table.at(name));
    }
  }
}

void CommandBuilder::AddOutput() {
  std::string name{"output"};

  if (m_Table.contains(name)) {
    std::string output{toml::find(m_Table, name).as_string()};
    if (output == "silent") {
      m_Command->Output = Command::Output::SILENT;
    } else if (output == "unchanged") {
      m_Command->Output = Command::Output::UNCHANGED;
    } else {
      m_Errors.emplace_back(
          ConfigurationErrorType::MALFORMED_COMMAND,
          fmt::format(R"(The "{}" can either be "unchanged" or "silent".)", name),
          m_Table.at(name));
    }
  }
}

void CommandBuilder::AddChildCommand(const Ref<Command>& command) {
  m_Command->ChildCommands.emplace_back(command);
}

}  // namespace Litr
