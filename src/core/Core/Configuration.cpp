#include "Configuration.hpp"

#include <iostream>

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Log.hpp"

namespace Litr {

Configuration::Configuration(const Path& filePath) {
  LITR_PROFILE_FUNCTION();

  m_RawConfig = toml::parse(filePath.ToString());

  if (!m_RawConfig.is_table()) {
    AppendError(ErrorType::MALFORMED_FILE, "Configuration is not a TOML table.");
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

std::vector<Configuration::Error> Configuration::GetErrors() const {
  return m_Errors;
}

bool Configuration::HasErrors() const {
  return !m_Errors.empty();
}

void Configuration::CollectCommands(const toml::table& commands) {
  LITR_PROFILE_FUNCTION();

  for (auto& [key, value] : commands) {
    Command command{key};
    LITR_CORE_TRACE("Creating {}", command);

    // Simple string form
    if (value.is_string()) {
      command.Script = {value.as_string()};
      m_Commands.emplace_back(command);
      continue;
    }

    // Simple string array form
    if (value.is_array()) {
      std::vector<std::string> scripts{};
      for (auto s : value.as_array()) {
        if (s.is_string()) {
          scripts.emplace_back(s.as_string());
        } else {
          AppendError(ErrorType::MALFORMED_SCRIPT, "A command script can be either a string or array of strings.",
                      commands.at(key));
        }
      }
      command.Script = scripts;
      m_Commands.emplace_back(command);
      continue;
    }

    // From here on it needs to be a table to be valid.
    if (!value.is_table()) {
      AppendError(ErrorType::MALFORMED_COMMAND, "A command can be a string or table.", commands.at(key));
      return;
    }

    // Detailed command form
    toml::value scripts{toml::find(value, "script")};

    if (scripts.is_string()) {
      command.Script = {scripts.as_string()};
    } else if (scripts.is_array()) {
      command.Script = {};
      for (auto s : scripts.as_array()) {
        command.Script.emplace_back(s.as_string());
      }
    }

    if (value.contains("description")) {
      toml::value description{toml::find(value, "description")};
      if (description.is_string()) {
        command.Description = description.as_string();
      } else {
        AppendError(ErrorType::MALFORMED_COMMAND, R"(The "description" can can only be a string.)",
                    value.at("description"));
      }
    }

    if (value.contains("example")) {
      toml::value example{toml::find(value, "example")};
      if (example.is_string()) {
        command.Description = example.as_string();
      } else {
        AppendError(ErrorType::MALFORMED_COMMAND, R"(The "example" can can only be a string.)",
                    value.at("example"));
      }
    }

    if (value.contains("dir")) {
      toml::value dir{toml::find(value, "dir")};
      if (dir.is_string()) {
        command.Directory.emplace_back(dir.as_string());
      } else if (dir.is_array()) {
        for (auto d : dir.as_array()) {
          command.Directory.emplace_back(d.as_string());
        }
      } else {
        AppendError(ErrorType::MALFORMED_COMMAND, R"(A "dir" can either be a string or array of strings.)",
                    value.at("dir"));
      }
    }

    if (value.contains("output")) {
      std::string output{toml::find(value, "output").as_string()};
      if (output == "silent") {
        command.Output = Command::Output::SILENT;
      } else if (output == "unchanged") {
        command.Output = Command::Output::UNCHANGED;
      } else {
        AppendError(ErrorType::MALFORMED_COMMAND, R"(The "output" can either be "unchanged" or "silent".)",
                    value.at("output"));
      }
    }

    m_Commands.emplace_back(command);
  }
}

void Configuration::CollectParams(const toml::table& params) {
  LITR_PROFILE_FUNCTION();

  for (auto& [key, value] : params) {
    if (key == "help") {
      AppendError(ErrorType::RESERVED_PARAM, "The parameter name \"help\" is restricted to Litr.", params.at(key));
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
      AppendError(ErrorType::MALFORMED_PARAM, "A parameter needs to be a string or table.", params.at(key));
      return;
    }

    // Detailed parameter form
    if (!value.contains("description")) {
      AppendError(ErrorType::MALFORMED_PARAM, R"(You're missing the "description" field.)", params.at(key));
      return;
    }

    toml::value description{toml::find(value, "description")};
    if (!description.is_string()) {
      AppendError(ErrorType::MALFORMED_PARAM, R"(The "description" can can only be a string.)",
                  value.at("description"));
      return;
    }

    param.Description = description.as_string();

    if (value.contains("shortcut")) {
      toml::value shortcut{toml::find(value, "shortcut")};
      if (shortcut.is_string()) {
        param.Shortcut = shortcut.as_string();
      } else {
        AppendError(ErrorType::MALFORMED_PARAM, R"(A "shortcut" can can only be a string.)",
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
        AppendError(ErrorType::MALFORMED_PARAM, R"(The field "default" needs to be a string.)", value.at("default"));
      }
    }

    m_Parameters.emplace_back(param);
  }
}

void Configuration::AppendError(Configuration::ErrorType type, const std::string& message) {
  LITR_PROFILE_FUNCTION();

  m_Errors.emplace_back(type, message);
}

void Configuration::AppendError(Configuration::ErrorType type, const std::string& message, const toml::value& context) {
  LITR_PROFILE_FUNCTION();

  Error err{type, message};
  toml::source_location location{context.location()};

  err.Line = location.line();
  err.Column = location.column();
  err.LineStr = location.line_str();

  m_Errors.emplace_back(err);
}

}  // namespace Litr
