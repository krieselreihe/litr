#include "Configuration.hpp"

#include <iostream>

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Log.hpp"
#include "Core/CommandBuilder.hpp"

namespace Litr {

Configuration::Configuration(const Path& filePath) {
  LITR_PROFILE_FUNCTION();

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

std::vector<Command> Configuration::GetCommands() const {
  return m_Commands;
}

std::vector<Parameter> Configuration::GetParameter() const {
  return m_Parameters;
}

std::vector<ConfigurationError> Configuration::GetErrors() const {
  return m_Errors;
}

bool Configuration::HasErrors() const {
  return !m_Errors.empty();
}

void Configuration::CollectCommands(const toml::table& commands) {
  LITR_PROFILE_FUNCTION();

  for (const auto& [key, value] : commands) {
    CommandBuilder builder{key, value, commands};

    // Simple string form
    if (value.is_string()) {
      builder.AddScriptLine(value.as_string());
      for (const auto& error : builder.GetErrors()) m_Errors.emplace_back(error);
      m_Commands.emplace_back(builder.GetResult());
      continue;
    }

    // Simple string array form
    if (value.is_array()) {
      builder.AddScript(value);
      for (const auto& error : builder.GetErrors()) m_Errors.emplace_back(error);
      m_Commands.emplace_back(builder.GetResult());
      continue;
    }

    // @todo: Check if sub command.

    // From here on it needs to be a table to be valid.
    if (!value.is_table()) {
      m_Errors.emplace_back(
          ConfigurationErrorType::MALFORMED_COMMAND,
          "A command can be a string or table.",
          commands.at(key));
      return;
    }

    // @todo: Until here there can be sub commands.

    // Detailed command form
    toml::value scripts{toml::find(value, "script")};
    if (scripts.is_string()) {
      builder.AddScriptLine(scripts.as_string());
    } else if (scripts.is_array()) {
      builder.AddScript(scripts);
    }

    builder.AddDescription();
    builder.AddExample();
    builder.AddDirectory();
    builder.AddOutput();

    for (const auto& error : builder.GetErrors()) m_Errors.emplace_back(error);
    m_Commands.emplace_back(builder.GetResult());
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
