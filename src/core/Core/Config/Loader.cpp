/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Loader.hpp"

#include <deque>
#include <utility>

#include "Core/Config/CommandBuilder.hpp"
#include "Core/Config/ParameterBuilder.hpp"
#include "Core/Debug/Instrumentor.hpp"
#include "Core/Error/Handler.hpp"
#include "Core/Log.hpp"
#include "Core/Utils.hpp"

namespace litr::config {

Loader::Loader(Path file_path) : m_file_path(std::move(file_path)) {
  LITR_PROFILE_FUNCTION();

  TomlFileAdapter::Value config{m_file.parse(m_file_path)};
  if (error::Handler::has_errors()) {
    return;
  }

  if (config.contains("commands")) {
    const TomlFileAdapter::Value& commands{m_file.find(config, "commands")};
    collect_commands(commands);
  }

  if (config.contains("params")) {
    const TomlFileAdapter::Value& params{m_file.find(config, "params")};
    collect_params(params);
  }
}

// NOLINTNEXTLINE(misc-no-recursion)
std::shared_ptr<Command> Loader::create_command(const TomlFileAdapter::Value& commands,
    const TomlFileAdapter::Value& definition,
    const std::string& name) {
  LITR_PROFILE_FUNCTION();

  CommandBuilder builder{commands, definition, name};

  // Simple string form
  if (definition.is_string()) {
    builder.add_script_line(definition.as_string(), definition);
    return builder.get_result();
  }

  // Simple string array form
  if (definition.is_array()) {
    builder.add_script(definition);
    return builder.get_result();
  }

  // From here on it needs to be a table to be valid.
  if (!definition.is_table()) {
    error::Handler::push(
        error::MalformedCommandError("A command can be a string or table.", commands.at(name)));
    return builder.get_result();
  }

  // Collect command property names
  std::deque<std::string> properties{};
  for (auto&& property : definition.as_table()) {
    properties.push_back(property.first);
  }

  while (!properties.empty()) {
    LITR_PROFILE_SCOPE("Config::Loader::create_command::CollectCommandProperties(while)");
    const std::string property{properties.front()};

    if (property == "script") {
      const TomlFileAdapter::Value& scripts{m_file.find(definition, "script")};

      if (scripts.is_string()) {
        builder.add_script_line(scripts.as_string(), scripts);
      } else if (scripts.is_array()) {
        builder.add_script(scripts);
      } else {
        error::Handler::push(error::MalformedScriptError(
            "A command script can be either a string or array of strings.",
            definition.at(property)));
      }

      properties.pop_front();
      continue;
    }

    if (property == "description") {
      builder.add_description();
      properties.pop_front();
      continue;
    }

    if (property == "example") {
      builder.add_example();
      properties.pop_front();
      continue;
    }

    if (property == "dir") {
      builder.add_directory(m_file_path.without_filename());
      properties.pop_front();
      continue;
    }

    if (property == "output") {
      builder.add_output();
      properties.pop_front();
      continue;
    }

    // Collect properties that cannot directly be resolved.
    const TomlFileAdapter::Value& value{m_file.find(definition, property)};
    if (!value.is_table()) {
      error::Handler::push(error::UnknownCommandPropertyError(
          fmt::format(
              R"(The command property "{}" does not exist. Please refer to the docs.)", property),
          definition.at(property)));
      properties.pop_front();
      continue;
    }

    // NOLINTNEXTLINE(misc-no-recursion)
    builder.add_child_command(create_command(definition.as_table(), value, property));
    properties.pop_front();
  }

  return builder.get_result();
}

void Loader::collect_commands(const TomlFileAdapter::Value& commands) {
  LITR_PROFILE_FUNCTION();

  if (commands.is_table()) {
    for (auto&& [name, definition] : commands.as_table()) {
      m_commands.emplace_back(create_command(commands, definition, name));
    }
  }
}

void Loader::collect_params(const TomlFileAdapter::Value& params) {
  LITR_PROFILE_FUNCTION();

  if (!params.is_table()) {
    return;
  }

  for (auto&& [name, definition] : params.as_table()) {
    ParameterBuilder builder{params, definition, name};

    if (ParameterBuilder::is_reserved_name(name)) {
      error::Handler::push(error::ReservedParamError(
          fmt::format(R"(The parameter name "{}" is reserved by Litr.)", name), params.at(name)));
      continue;
    }

    // Simple string form
    if (definition.is_string()) {
      builder.add_description(definition.as_string());
      m_parameters.emplace_back(builder.get_result());
      continue;
    }

    // From here on it needs to be a table to be valid.
    if (!definition.is_table()) {
      error::Handler::push(error::MalformedParamError(
          "A parameter needs to be a string or table.", params.at(name)));
      continue;
    }

    builder.add_description();
    builder.add_shortcut(m_parameters);
    builder.add_type();
    builder.add_default();

    m_parameters.emplace_back(builder.get_result());
  }
}

}  // namespace litr::config
