#include "ConfigLoader.hpp"

#include <iostream>

#include "Core/Config/CommandBuilder.hpp"
#include "Core/Config/ParameterBuilder.hpp"
#include "Core/Debug/Instrumentor.hpp"
#include "Core/Log.hpp"

namespace Litr {

ConfigLoader::ConfigLoader(const Path& filePath) {
  LITR_PROFILE_FUNCTION();

  // @todo: Command order is currently "unordered". Needs to be ordered.
  toml::value config{toml::parse(filePath.ToString())};

  if (!config.is_table()) {
    m_Errors.emplace_back(
        ConfigurationErrorType::MALFORMED_FILE,
        "Configuration is not a TOML table.");
    return;
  }

  if (config.contains("commands")) {
    auto& commands{toml::find<toml::table>(config, "commands")};
    CollectCommands(commands);
  }

  if (config.contains("params")) {
    auto& params{toml::find<toml::table>(config, "params")};
    CollectParams(params);
  }
}

// Ignore recursion warning.
// NOLINTNEXTLINE
Ref<Command> ConfigLoader::CreateCommand(const toml::table& commands, const toml::value& definition, const std::string& name) {
  LITR_PROFILE_FUNCTION();

  CommandBuilder builder{commands, definition, name};

  // Simple string form
  if (definition.is_string()) {
    builder.AddScriptLine(definition.as_string());
    for (const auto& error : builder.GetErrors()) m_Errors.emplace_back(error);
    return builder.GetResult();
  }

  // Simple string array form
  if (definition.is_array()) {
    builder.AddScript(definition);
    for (const auto& error : builder.GetErrors()) m_Errors.emplace_back(error);
    return builder.GetResult();
  }

  // From here on it needs to be a table to be valid.
  if (!definition.is_table()) {
    m_Errors.emplace_back(
        ConfigurationErrorType::MALFORMED_COMMAND,
        "A command can be a string or table.",
        commands.at(name));
    for (const auto& error : builder.GetErrors()) m_Errors.emplace_back(error);
    return builder.GetResult();
  }

  // Collect command property names
  std::stack<std::string> properties{};
  for (const auto& property : definition.as_table()) {
    properties.push(property.first);
  }

  while (!properties.empty()) {
    LITR_PROFILE_SCOPE("ConfigLoader::CreateCommand::CollectCommandProperties(while)");
    const std::string property{properties.top()};

    if (property == "script") {
      toml::value scripts{toml::find(definition, "script")};

      if (scripts.is_string()) {
        builder.AddScriptLine(scripts.as_string());
      } else if (scripts.is_array()) {
        builder.AddScript(scripts);
      } else {
        m_Errors.emplace_back(
            ConfigurationErrorType::MALFORMED_SCRIPT,
            "A command script can be either a string or array of strings.",
            definition.at(property));
      }

      properties.pop();
      continue;
    }

    if (property == "description") {
      builder.AddDescription();
      properties.pop();
      continue;
    }

    if (property == "example") {
      builder.AddExample();
      properties.pop();
      continue;
    }

    if (property == "dir") {
      builder.AddDirectory();
      properties.pop();
      continue;
    }

    if (property == "output") {
      builder.AddOutput();
      properties.pop();
      continue;
    }

    // Collect properties that cannot directly be resolved.
    const toml::value& value{toml::find(definition, property)};
    if (!value.is_table()) {
      m_Errors.emplace_back(
          ConfigurationErrorType::UNKNOWN_COMMAND_PROPERTY,
          fmt::format(R"(The command property "{}" does not exist. Please refer to the docs.)", property),
          definition.at(property));
      properties.pop();
      continue;
    }

    // Ignore recursion warning.
    // NOLINTNEXTLINE
    builder.AddChildCommand(CreateCommand(definition.as_table(), value, property));
    properties.pop();
  }

  for (const auto& error : builder.GetErrors()) m_Errors.emplace_back(error);
  return builder.GetResult();
}

void ConfigLoader::CollectCommands(const toml::table& commands) {
  LITR_PROFILE_FUNCTION();

  for (const auto& [name, definition] : commands) {
    m_Commands.emplace_back(CreateCommand(commands, definition, name));
  }
}

void ConfigLoader::CollectParams(const toml::table& params) {
  LITR_PROFILE_FUNCTION();

  for (auto& [name, definition] : params) {
    if (IsReservedParameter(name)) {
      m_Errors.emplace_back(
          ConfigurationErrorType::RESERVED_PARAM,
          fmt::format(R"(The parameter name "{}" is reserved by Litr.)", name),
          params.at(name));
      continue;
    }

    ParameterBuilder builder{params, definition, name};

    // Simple string form
    if (definition.is_string()) {
      builder.AddDescription(definition.as_string());
      m_Parameters.emplace_back(builder.GetResult());
      continue;
    }

    // From here on it needs to be a table to be valid.
    if (!definition.is_table()) {
      m_Errors.emplace_back(
          ConfigurationErrorType::MALFORMED_PARAM,
          "A parameter needs to be a string or table.",
          params.at(name));
      continue;
    }

    builder.AddDescription();
    builder.AddShortcut();
    builder.AddType();
    builder.AddDefault();

    m_Parameters.emplace_back(builder.GetResult());
  }
}

bool ConfigLoader::IsReservedParameter(const std::string& name) {
  const std::array<std::string, 2> reserved{"help", "h"};

  return std::find(reserved.begin(), reserved.end(), name) != reserved.end();
}

}  // namespace Litr
