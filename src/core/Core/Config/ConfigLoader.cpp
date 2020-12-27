#include "ConfigLoader.hpp"

#include <tsl/ordered_map.h>

#include "Core/Config/CommandBuilder.hpp"
#include "Core/Config/ParameterBuilder.hpp"
#include "Core/Debug/Instrumentor.hpp"
#include "Core/Log.hpp"

namespace Litr {

static Ref<Command> GetCommandByName(const std::string& name, const std::vector<Ref<Command>>& commands) {
  for (const Ref<Command>& command : commands) {
    if (command->Name == name) {
      return command;
    }
  }

  return nullptr;
}

// Ignore recursion warning.
// NOLINTNEXTLINE
static Ref<Command> ResolveCommandByPath(std::deque<std::string>& names, const std::vector<Ref<Command>>& commands) {
  const Ref<Command>& command{GetCommandByName(names.front(), commands)};

  if (command == nullptr) {
    return nullptr;
  }

  names.pop_front();

  if (names.empty()) {
    return command;
  }

  if (command->ChildCommands.empty()) {
    if (!names.empty()) {
      // @todo: This is an error, but not a hard one as the user could access sub commands that
      // do not exist. But I also do not want to emit a user error as this is not the place for
      // this. Maybe this whole `GetCommand` function should not exist here and rather be moved
      // to the client.
    }

    return nullptr;
  }

  // Ignore recursion warning.
  // NOLINTNEXTLINE
  return ResolveCommandByPath(names, command->ChildCommands);
}

static std::deque<std::string> SplitCommandQuery(const std::string& query) {
  std::stringstream ss{query};
  std::string name;
  std::deque<std::string> parts;

  while (std::getline(ss, name, '.')) {
    parts.push_back(name);
  }

  return parts;
}

ConfigLoader::ConfigLoader(const Ref<ErrorHandler>& errorHandler, const Path& filePath) : m_ErrorHandler(errorHandler) {
  LITR_PROFILE_FUNCTION();

  toml::basic_value<toml::discard_comments, tsl::ordered_map> config{};

  try {
    config = toml::parse<toml::discard_comments, tsl::ordered_map>(filePath.ToString());
  } catch (const toml::syntax_error& err) {
    m_ErrorHandler->Push({
      ErrorType::MALFORMED_FILE,
      "There is a syntax error inside the configuration file.",
      err
    });
    return;
  }

  if (!config.is_table()) {
    m_ErrorHandler->Push({
      ErrorType::MALFORMED_FILE,
      "Configuration is not a TOML table."
    });
    return;
  }

  if (config.contains("commands")) {
    const toml::table& commands{toml::find<toml::table, toml::discard_comments, tsl::ordered_map>(config, "commands")};
    CollectCommands(commands);
  }

  if (config.contains("params")) {
    const toml::table& params{toml::find<toml::table, toml::discard_comments, tsl::ordered_map>(config, "params")};
    CollectParams(params);
  }
}

Ref<Command> ConfigLoader::GetCommand(const std::string& name) const {
  std::deque names{SplitCommandQuery(name)};
  return ResolveCommandByPath(names, m_Commands);
}

Ref<Parameter> ConfigLoader::GetParameter(const std::string& name) const {
  for (const Ref<Parameter>& param : m_Parameters) {
    if (param->Name == name) {
      return param;
    }
  }

  return nullptr;
}

// Ignore recursion warning.
// NOLINTNEXTLINE
Ref<Command> ConfigLoader::CreateCommand(const toml::table& commands, const toml::value& definition, const std::string& name) {
  LITR_PROFILE_FUNCTION();

  CommandBuilder builder{m_ErrorHandler, commands, definition, name};

  // Simple string form
  if (definition.is_string()) {
    builder.AddScriptLine(definition.as_string());
    return builder.GetResult();
  }

  // Simple string array form
  if (definition.is_array()) {
    builder.AddScript(definition);
    return builder.GetResult();
  }

  // From here on it needs to be a table to be valid.
  if (!definition.is_table()) {
    m_ErrorHandler->Push({
      ErrorType::MALFORMED_COMMAND,
      "A command can be a string or table.",
      commands.at(name)
    });
    return builder.GetResult();
  }

  // Collect command property names
  std::stack<std::string> properties{};
  for (auto&& property : definition.as_table()) {
    properties.push(property.first);
  }

  while (!properties.empty()) {
    LITR_PROFILE_SCOPE("ConfigLoader::CreateCommand::CollectCommandProperties(while)");
    const std::string property{properties.top()};

    if (property == "script") {
      const toml::value& scripts{toml::find(definition, "script")};

      if (scripts.is_string()) {
        builder.AddScriptLine(scripts.as_string());
      } else if (scripts.is_array()) {
        builder.AddScript(scripts);
      } else {
        m_ErrorHandler->Push({
          ErrorType::MALFORMED_SCRIPT,
          "A command script can be either a string or array of strings.",
          definition.at(property)
        });
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
      m_ErrorHandler->Push({
        ErrorType::UNKNOWN_COMMAND_PROPERTY,
        fmt::format(R"(The command property "{}" does not exist. Please refer to the docs.)", property),
        definition.at(property)
      });
      properties.pop();
      continue;
    }

    // Ignore recursion warning.
    // NOLINTNEXTLINE
    builder.AddChildCommand(CreateCommand(definition.as_table(), value, property));
    properties.pop();
  }

  return builder.GetResult();
}

void ConfigLoader::CollectCommands(const toml::table& commands) {
  LITR_PROFILE_FUNCTION();

  for (auto&& [name, definition] : commands) {
    m_Commands.emplace_back(CreateCommand(commands, definition, name));
  }
}

void ConfigLoader::CollectParams(const toml::table& params) {
  LITR_PROFILE_FUNCTION();

  for (auto&& [name, definition] : params) {
    ParameterBuilder builder{m_ErrorHandler, params, definition, name};

    if (ParameterBuilder::IsReservedName(name)) {
      m_ErrorHandler->Push({
        ErrorType::RESERVED_PARAM,
        fmt::format(R"(The parameter name "{}" is reserved by Litr.)", name),
        params.at(name)
      });
      continue;
    }

    // Simple string form
    if (definition.is_string()) {
      builder.AddDescription(definition.as_string());
      m_Parameters.emplace_back(builder.GetResult());
      continue;
    }

    // From here on it needs to be a table to be valid.
    if (!definition.is_table()) {
      m_ErrorHandler->Push({
        ErrorType::MALFORMED_PARAM,
        "A parameter needs to be a string or table.",
        params.at(name)
      });
      continue;
    }

    builder.AddDescription();
    builder.AddShortcut();
    builder.AddType();
    builder.AddDefault();

    m_Parameters.emplace_back(builder.GetResult());
  }
}

}  // namespace Litr
