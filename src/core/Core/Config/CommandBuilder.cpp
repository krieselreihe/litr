#include "CommandBuilder.hpp"

#include <fmt/format.h>

#include "Core/Error/Handler.hpp"

namespace Litr::Config {

CommandBuilder::CommandBuilder(const toml::table& file, const toml::value& data, const std::string& name)
    : m_File(file), m_Table(data), m_Command(CreateRef<Command>(name)) {
  LITR_CORE_TRACE("Creating {}", *m_Command);
}

void CommandBuilder::AddScriptLine(const std::string& line) {
  LITR_PROFILE_FUNCTION();

  m_Command->Script.emplace_back(line);
}

void CommandBuilder::AddScriptLine(const std::string& line, const toml::value& context) {
  AddScriptLine(line);
  AddLocation(context);
}

void CommandBuilder::AddScript(const std::vector<std::string>& scripts) {
  LITR_PROFILE_FUNCTION();

  m_Command->Script = scripts;
}

void CommandBuilder::AddScript(const std::vector<std::string>& scripts, const toml::value& context) {
  AddScript(scripts);
  for (auto&& entry : context.as_array()) AddLocation(entry);
}

void CommandBuilder::AddScript(const toml::value& scripts) {
  LITR_PROFILE_FUNCTION();

  for (auto&& script : scripts.as_array()) {
    if (!script.is_string()) {
      Error::Handler::Push(Error::MalformedScriptError(
          "A command script can be either a string or array of strings.",
          m_File.at(m_Command->Name)
      ));
      // Stop after first error in an array of scripts, to avoid being too verbose.
      break;
    }

    AddScriptLine(script.as_string(), script);
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

    Error::Handler::Push(Error::MalformedCommandError(
        fmt::format(R"(The "{}" can only be a string.)", name),
        m_Table.at(name)
    ));
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

    Error::Handler::Push(Error::MalformedCommandError(
        fmt::format(R"(The "{}" can only be a string.)", name),
        m_Table.at(name)
    ));
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
      for (auto&& directory : directories.as_array()) {
        if (!directory.is_string()) {
          Error::Handler::Push(Error::MalformedCommandError(
              fmt::format(R"(A "{}" can either be a string or array of strings.)", name),
              m_Table.at(name)
          ));
          continue;
        }

        m_Command->Directory.emplace_back(directory.as_string());
      }
      return;
    }

    Error::Handler::Push(Error::MalformedCommandError(
        fmt::format(R"(A "{}" can either be a string or array of strings.)", name),
        m_Table.at(name)
    ));
  }
}

void CommandBuilder::AddOutput() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"output"};

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

    Error::Handler::Push(Error::MalformedCommandError(
        fmt::format(R"(The "{}" can either be "unchanged" or "silent".)", name),
        m_Table.at(name)
    ));
  }
}

void CommandBuilder::AddChildCommand(const Ref<Command>& command) {
  LITR_PROFILE_FUNCTION();

  m_Command->ChildCommands.emplace_back(command);
}

void CommandBuilder::AddLocation(const toml::value& context) {
  LITR_PROFILE_FUNCTION();

  m_Command->Locations.emplace_back(
      context.location().line(),
      context.location().column(),
      context.location().line_str()
  );
}

}  // namespace Litr::Config
