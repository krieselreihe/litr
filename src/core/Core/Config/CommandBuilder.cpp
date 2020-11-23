#include "CommandBuilder.hpp"

#include <fmt/format.h>

namespace Litr {

CommandBuilder::CommandBuilder(const toml::table& file, const toml::value& data, const std::string& name) : m_File(file), m_Table(data) {
  m_Command = CreateRef<Command>(name);
  LITR_CORE_TRACE("Creating {}", *m_Command);
}

void CommandBuilder::AddScriptLine(const std::string& line) {
  LITR_PROFILE_FUNCTION();

  m_Command->Script.emplace_back(line);
}

void CommandBuilder::AddScript(const std::vector<std::string>& scripts) {
  LITR_PROFILE_FUNCTION();

  m_Command->Script = scripts;
}

void CommandBuilder::AddScript(const toml::value& scripts) {
  LITR_PROFILE_FUNCTION();

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
  LITR_PROFILE_FUNCTION();

  const std::string name{"description"};

  if (m_Table.contains(name)) {
    const toml::value& description{toml::find(m_Table, name)};

    if (description.is_string()) {
      m_Command->Description = description.as_string();
      return;
    }

    m_Errors.emplace_back(
        ConfigurationErrorType::MALFORMED_COMMAND,
        fmt::format(R"(The "{}" can can only be a string.)", name),
        m_Table.at(name));
  }
}

void CommandBuilder::AddExample() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"example"};

  if (m_Table.contains(name)) {
    const toml::value& example{toml::find(m_Table, name)};
    if (example.is_string()) {
      m_Command->Example = example.as_string();
      return;
    }

    m_Errors.emplace_back(
        ConfigurationErrorType::MALFORMED_COMMAND,
        fmt::format(R"(The "{}" can can only be a string.)", name),
        m_Table.at(name));
  }
}

void CommandBuilder::AddDirectory() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"dir"};

  if (m_Table.contains(name)) {
    const toml::value& directories{toml::find(m_Table, name)};

    if (directories.is_string()) {
      m_Command->Directory.emplace_back(directories.as_string());
      return;
    }

    if (directories.is_array()) {
      for (const auto& directory : directories.as_array()) {
        if (!directory.is_string()) {
          m_Errors.emplace_back(
              ConfigurationErrorType::MALFORMED_COMMAND,
              fmt::format(R"(A "{}" can either be a string or array of strings.)", name),
              m_Table.at(name));
          continue;
        }

        m_Command->Directory.emplace_back(directory.as_string());
      }
      return;
    }

    m_Errors.emplace_back(
        ConfigurationErrorType::MALFORMED_COMMAND,
        fmt::format(R"(A "{}" can either be a string or array of strings.)", name),
        m_Table.at(name));
  }
}

void CommandBuilder::AddOutput() {
  LITR_PROFILE_FUNCTION();

  std::string name{"output"};

  if (m_Table.contains(name)) {
    const toml::value& output{toml::find(m_Table, name)};

    if (output.is_string()) {
      if (output.as_string() == "silent") {
        m_Command->Output = Command::Output::SILENT;
        return;
      }

      if (output.as_string() == "unchanged") {
        m_Command->Output = Command::Output::UNCHANGED;
        return;
      }
    }

    m_Errors.emplace_back(
        ConfigurationErrorType::MALFORMED_COMMAND,
        fmt::format(R"(The "{}" can either be "unchanged" or "silent".)", name),
        m_Table.at(name));
  }
}

void CommandBuilder::AddChildCommand(const Ref<Command>& command) {
  LITR_PROFILE_FUNCTION();

  m_Command->ChildCommands.emplace_back(command);
}

}  // namespace Litr
