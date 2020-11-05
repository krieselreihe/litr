#include "Configuration.hpp"

#include <iostream>

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Log.hpp"
#include "Core/CommandBuilder.hpp"

namespace Litr {

Configuration::Configuration(const Path& filePath) {
  LITR_PROFILE_FUNCTION();

  // @todo: Command order is currently "unordered". Needs to be ordered.
  m_RawConfig = toml::parse(filePath.ToString());

  if (!m_RawConfig.is_table()) {
    m_Errors.emplace_back(
        ConfigurationErrorType::MALFORMED_FILE,
        "Configuration is not a TOML table.");
    return;
  }

  if (m_RawConfig.contains("commands")) {
    auto& commands{toml::find<toml::table>(m_RawConfig, "commands")};
    CollectCommands(commands);
  }

  if (m_RawConfig.contains("params")) {
    auto& params{toml::find<toml::table>(m_RawConfig, "params")};
    CollectParams(params);
  }
}

// Ignore recursion warning.
// NOLINTNEXTLINE
Ref<Command> Configuration::CreateCommand(const toml::table& commands, const toml::value& definition, const std::string& name) {
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

  std::stack<std::string> extraProperties{};
  while (!properties.empty()) {
    LITR_PROFILE_SCOPE("Configuration::CreateCommand::CollectCommandProperties(while)");
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
    extraProperties.push(property);
    properties.pop();
  }

  while (!extraProperties.empty()) {
    LITR_PROFILE_SCOPE("Configuration::CreateCommand::AddSubCommands(while)");
    const std::string property{extraProperties.top()};
    const toml::value& value{toml::find(definition, property)};

    if (!value.is_table()) {
      m_Errors.emplace_back(
          ConfigurationErrorType::UNKNOWN_COMMAND_PROPERTY,
          fmt::format(R"(The command property "{}" does not exist. Please refer to the docs.)", property),
          definition.at(property));
      extraProperties.pop();
      continue;
    }

    // Ignore recursion warning.
    // NOLINTNEXTLINE
    builder.AddChildCommand(CreateCommand(definition.as_table(), value, property));
    extraProperties.pop();
  }

  for (const auto& error : builder.GetErrors()) m_Errors.emplace_back(error);
  return builder.GetResult();
}

void Configuration::CollectCommands(const toml::table& commands) {
  LITR_PROFILE_FUNCTION();

  for (const auto& [name, definition] : commands) {
    m_Commands.emplace_back(CreateCommand(commands, definition, name));
  }
}

void Configuration::CollectParams(const toml::table& params) {
  LITR_PROFILE_FUNCTION();

  for (auto& [key, value] : params) {
    // @todo: Flexible restricted field names.
    if (key == "help") {
      m_Errors.emplace_back(
          ConfigurationErrorType::RESERVED_PARAM,
          "The parameter name \"help\" is restricted to Litr.",
          params.at(key));
      continue;
    }

    // @todo: Use same builder pattern as for commands.
    Parameter param{key};
    LITR_CORE_TRACE("Creating {}", param);

    // Simple string form
    if (value.is_string()) {
      param.Description = value.as_string();
      m_Parameters.emplace_back(param);
      continue;
    }

    // From here on it needs to be a table to be valid.
    if (!value.is_table()) {
      m_Errors.emplace_back(
          ConfigurationErrorType::MALFORMED_PARAM,
          "A parameter needs to be a string or table.",
          params.at(key));
      return;
    }

    // Detailed parameter form
    if (!value.contains("description")) {
      m_Errors.emplace_back(
          ConfigurationErrorType::MALFORMED_PARAM,
          R"(You're missing the "description" field.)",
          params.at(key));
      return;
    }

    toml::value description{toml::find(value, "description")};
    if (!description.is_string()) {
      m_Errors.emplace_back(
          ConfigurationErrorType::MALFORMED_PARAM,
          R"(The "description" can can only be a string.)",
          value.at("description"));
      return;
    }

    param.Description = description.as_string();

    if (value.contains("shortcut")) {
      toml::value shortcut{toml::find(value, "shortcut")};
      if (shortcut.is_string()) {
        param.Shortcut = shortcut.as_string();
      } else {
        m_Errors.emplace_back(
            ConfigurationErrorType::MALFORMED_PARAM,
            R"(A "shortcut" can can only be a string.)",
            value.at("shortcut"));
      }
    }

    if (value.contains("type")) {
      if (value.is_array()) {
        param.Type = Parameter::ParameterType::Array;
        for (auto pa : value.as_array()) {
          param.TypeArguments.emplace_back(pa.as_string());
        }
      }
    }

    if (value.contains("default")) {
      toml::value def{toml::find(value, "default")};
      if (def.is_string()) {
        param.Default = def.as_string();
      } else {
        m_Errors.emplace_back(
            ConfigurationErrorType::MALFORMED_PARAM,
            R"(The field "default" needs to be a string.)",
            value.at("default"));
      }
    }

    m_Parameters.emplace_back(param);
  }
}

}  // namespace Litr
