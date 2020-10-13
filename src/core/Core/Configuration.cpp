#include "Configuration.hpp"

#include <fstream> //required for parse_file()

#include "Core/Log.hpp"

namespace Litr {

Configuration::Configuration(const Path& filePath) {
  toml::parse_result result{toml::parse_file(filePath.ToString())};

  if (!result) {
    // @todo: Errors needs to be parsed and wrapped to be maximum helpful to the user.
    // https://github.com/krieselreihe/litr/issues/11
    LITR_ERROR("Parsing failed in {} with {}", filePath, result.error().description());
    return;
  }

  m_RawConfig = std::move(result).table();

  if (!m_RawConfig.is_table()) {
    LITR_ERROR("Configuration is not a TOML table.");
    return;
  }

  for (auto c : m_RawConfig) {
    if (c.first == "commands") CollectCommands();
    if (c.first == "params") CollectParams();
  }
}

void Configuration::CollectCommands() {
  toml::node_view commands{m_RawConfig["commands"]};

  if (!commands.is_table()) {
    LITR_ERROR("Commands is not a TOML table.");
    return;
  }

  for (auto&& [key, _value] : *commands.as_table()) {
    auto cmd{commands[key]};

    // Simple string form
    if (cmd.is_string()) {
      std::vector<std::string> scripts{{cmd.value_or("")}};
      Command command{key, scripts};
      m_Commands.emplace_back(command);
      continue;
    }

    // Simple string array form
    if (cmd.is_array()) {
      std::vector<std::string> scripts{};
      for (auto& t : *cmd.as_array()) {
        scripts.emplace_back(t.value_or(""));
      }
      Command command{key, scripts};
      m_Commands.emplace_back(command);
      continue;
    }

    // Detailed command form
    std::vector<std::string> scripts{cmd["script"].value_or("")};
    std::string description{cmd["description"].value_or("")};
    std::string example{cmd["example"].value_or("")};
    // @todo: Missing options: dir and output.

    Command command{key, scripts, description, example};
    m_Commands.emplace_back(command);
  }
}

void Configuration::CollectParams() {
  toml::node_view params{m_RawConfig["params"]};

  if (!params.is_table()) {
    LITR_ERROR("Params is not a TOML table.");
    return;
  }

  // @todo: Collect and store params.
}

}  // namespace Litr
