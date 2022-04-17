/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "CommandBuilder.hpp"

#include <fmt/format.h>

#include "Core/Error/Handler.hpp"

namespace Litr::Config {

CommandBuilder::CommandBuilder(const TomlFileAdapter::Value& context,
    const TomlFileAdapter::Value& data,
    const std::string& name)
    : m_context(context),
      m_table(data),
      m_command(std::make_shared<Command>(name)) {
  LITR_CORE_TRACE("Creating {}", *m_command);
}

void CommandBuilder::add_script_line(const std::string& line) {
  LITR_PROFILE_FUNCTION();

  m_command->script.emplace_back(line);
}

void CommandBuilder::add_script_line(
    const std::string& line, const TomlFileAdapter::Value& context) {
  add_script_line(line);
  add_location(context);
}

void CommandBuilder::add_script(const std::vector<std::string>& scripts) {
  LITR_PROFILE_FUNCTION();

  m_command->script = scripts;
}

void CommandBuilder::add_script(const TomlFileAdapter::Value& scripts) {
  LITR_PROFILE_FUNCTION();

  for (auto&& script : scripts.as_array()) {
    if (!script.is_string()) {
      Error::Handler::push(Error::MalformedScriptError(
          "A command script can be either a string or array of strings.",
          m_context.at(m_command->name)));
      // Stop after first error in an array of scripts, to avoid being too verbose.
      break;
    }

    add_script_line(script.as_string(), script);
  }
}

void CommandBuilder::add_description() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"description"};

  if (m_table.contains(name)) {
    const TomlFileAdapter::Value& description{m_file.find(m_table, name)};

    if (description.is_string()) {
      m_command->description = description.as_string();
      return;
    }

    Error::Handler::push(Error::MalformedCommandError(
        fmt::format(R"(The "{}" can only be a string.)", name), m_table.at(name)));
  }
}

void CommandBuilder::add_example() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"example"};

  if (m_table.contains(name)) {
    const TomlFileAdapter::Value& example{m_file.find(m_table, name)};
    if (example.is_string()) {
      m_command->example = example.as_string();
      return;
    }

    Error::Handler::push(Error::MalformedCommandError(
        fmt::format(R"(The "{}" can only be a string.)", name), m_table.at(name)));
  }
}

void CommandBuilder::add_directory(const Path& root) {
  LITR_PROFILE_FUNCTION();

  const std::string name{"dir"};

  if (m_table.contains(name)) {
    const TomlFileAdapter::Value& directories{m_file.find(m_table, name)};

    if (directories.is_string()) {
      m_command->directory.emplace_back(root.append(directories.as_string()));
      return;
    }

    if (directories.is_array()) {
      for (auto&& directory : directories.as_array()) {
        if (!directory.is_string()) {
          Error::Handler::push(Error::MalformedCommandError(
              fmt::format(R"(A "{}" can either be a string or array of strings.)", name),
              m_table.at(name)));
          continue;
        }

        m_command->directory.emplace_back(root.append(directory.as_string()));
      }
      return;
    }

    Error::Handler::push(Error::MalformedCommandError(
        fmt::format(R"(A "{}" can either be a string or array of strings.)", name),
        m_table.at(name)));
  }
}

void CommandBuilder::add_output() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"output"};

  if (m_table.contains(name)) {
    const TomlFileAdapter::Value& output{m_file.find(m_table, name)};

    if (output.is_string()) {
      if (output.as_string() == "silent") {
        m_command->output = Command::Output::SILENT;
        return;
      }

      if (output.as_string() == "unchanged") {
        m_command->output = Command::Output::UNCHANGED;
        return;
      }
    }

    Error::Handler::push(Error::MalformedCommandError(
        fmt::format(R"(The "{}" can either be "unchanged" or "silent".)", name), m_table.at(name)));
  }
}

void CommandBuilder::add_child_command(const std::shared_ptr<Command>& command) {
  LITR_PROFILE_FUNCTION();

  m_command->child_commands.emplace_back(command);
}

void CommandBuilder::add_location(const TomlFileAdapter::Value& context) {
  LITR_PROFILE_FUNCTION();

  m_command->locations.emplace_back(
      context.location().line(), context.location().column(), context.location().line_str());
}

}  // namespace Litr::Config
