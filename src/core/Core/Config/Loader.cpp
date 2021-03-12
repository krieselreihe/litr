#include "Loader.hpp"

#include <tsl/ordered_map.h>
#include <stack>

#include "Core/Config/CommandBuilder.hpp"
#include "Core/Config/ParameterBuilder.hpp"
#include "Core/Debug/Instrumentor.hpp"
#include "Core/Error/Handler.hpp"
#include "Core/Log.hpp"
#include "Core/Utils.hpp"

namespace Litr::Config {

Loader::Loader(const Path& filePath) {
  LITR_PROFILE_FUNCTION();

  toml::basic_value<toml::discard_comments, tsl::ordered_map> config{};

  try {
    config = toml::parse<toml::discard_comments, tsl::ordered_map>(filePath.ToString());
  } catch (const toml::syntax_error& err) {
    Error::Handler::Push(Error::MalformedFileError(
        "There is a syntax error inside the configuration file.",
        err
    ));
    return;
  }

  if (!config.is_table()) {
    Error::Handler::Push(Error::MalformedFileError(
        "Configuration is not a TOML table."
    ));
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

// Ignore recursion warning.
// NOLINTNEXTLINE
Ref<Command> Loader::CreateCommand(const toml::table& commands, const toml::value& definition, const std::string& name) {
  LITR_PROFILE_FUNCTION();

  CommandBuilder builder{commands, definition, name};

  // Simple string form
  if (definition.is_string()) {
    builder.AddScriptLine(definition.as_string(), definition);
    return builder.GetResult();
  }

  // Simple string array form
  if (definition.is_array()) {
    builder.AddScript(definition);
    return builder.GetResult();
  }

  // From here on it needs to be a table to be valid.
  if (!definition.is_table()) {
    Error::Handler::Push(Error::MalformedCommandError(
        "A command can be a string or table.",
        commands.at(name)
    ));
    return builder.GetResult();
  }

  // Collect command property names
  std::stack<std::string> properties{};
  for (auto&& property : definition.as_table()) {
    properties.push(property.first);
  }

  while (!properties.empty()) {
    LITR_PROFILE_SCOPE("Config::Loader::CreateCommand::CollectCommandProperties(while)");
    const std::string property{properties.top()};

    if (property == "script") {
      const toml::value& scripts{toml::find(definition, "script")};

      if (scripts.is_string()) {
        builder.AddScriptLine(scripts.as_string(), scripts);
      } else if (scripts.is_array()) {
        builder.AddScript(scripts);
      } else {
        Error::Handler::Push(Error::MalformedScriptError(
            "A command script can be either a string or array of strings.",
            definition.at(property)
        ));
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
      Error::Handler::Push(Error::UnknownCommandPropertyError(
          fmt::format(R"(The command property "{}" does not exist. Please refer to the docs.)", property),
          definition.at(property)
      ));
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

void Loader::CollectCommands(const toml::table& commands) {
  LITR_PROFILE_FUNCTION();

  for (auto&& [name, definition] : commands) {
    m_Commands.emplace_back(CreateCommand(commands, definition, name));
  }
}

void Loader::CollectParams(const toml::table& params) {
  LITR_PROFILE_FUNCTION();

  for (auto&& [name, definition] : params) {
    ParameterBuilder builder{params, definition, name};

    if (ParameterBuilder::IsReservedName(name)) {
      Error::Handler::Push(Error::ReservedParamError(
          fmt::format(R"(The parameter name "{}" is reserved by Litr.)", name),
          params.at(name)
      ));
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
      Error::Handler::Push(Error::MalformedParamError(
          "A parameter needs to be a string or table.",
          params.at(name)
      ));
      continue;
    }

    builder.AddDescription();
    builder.AddShortcut();
    builder.AddType();
    builder.AddDefault();

    m_Parameters.emplace_back(builder.GetResult());
  }
}

}  // namespace Litr::Config
