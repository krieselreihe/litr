#include "Query.hpp"

#include <algorithm>

#include "Core/Utils.hpp"
#include "Core/Debug/Instrumentor.hpp"
#include "Core/Script/Compiler.hpp"

namespace Litr::Config {

Query::Query(const Ref<Loader>& config) : m_Config(config) {
}

Ref<Command> Query::GetCommand(const std::string& name) const {
  LITR_PROFILE_FUNCTION();

  Parts names{SplitCommandQuery(name)};
  return GetCommandByPath(names, GetCommands());
}

Ref<Parameter> Query::GetParameter(const std::string& name) const {
  LITR_PROFILE_FUNCTION();

  for (auto&& param : GetParameters()) {
    if (param->Name == name || param->Shortcut == name) {
      return param;
    }
  }

  return nullptr;
}

Query::Commands Query::GetCommands() const {
  LITR_PROFILE_FUNCTION();

  return m_Config->GetCommands();
}

Query::Parameters Query::GetParameters() const {
  LITR_PROFILE_FUNCTION();

  return m_Config->GetParameters();
}

std::vector<std::string> Query::GetUsedCommandParameters(const std::string& name) const {
  auto command{GetCommand(name)};
  std::vector<std::string> parameterNames{};

  size_t index{0};
  for (auto&& script : command->Script) {
    Variables variables{GetParametersAsVariables()};
    Script::Compiler compiler{script, command->Locations[index++], variables};
    std::vector<std::string> names{compiler.GetUsedVariables()};

    if (!names.empty()) {
      parameterNames.insert(parameterNames.end(), names.begin(), names.end());
    }
  }

  // Remove duplicates
  parameterNames.erase(std::unique(parameterNames.begin(), parameterNames.end()), parameterNames.end());

  return parameterNames;
}

Query::Parts Query::SplitCommandQuery(const std::string& query) {
  LITR_PROFILE_FUNCTION();

  Parts parts{};
  Utils::SplitInto(query, '.', parts);
  return parts;
}

// Ignore recursion warning.
// NOLINTNEXTLINE
Ref<Command> Query::GetCommandByPath(Parts& names, const Loader::Commands& commands) {
  LITR_PROFILE_FUNCTION();

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
      // Do nothing. This will lead further down the line to a "sub command not found" error.
    }

    return nullptr;
  }

  // Ignore recursion warning.
  // NOLINTNEXTLINE
  return GetCommandByPath(names, command->ChildCommands);
}

Ref<Command> Query::GetCommandByName(const std::string& name, const Loader::Commands& commands) {
  LITR_PROFILE_FUNCTION();

  for (const Ref<Command>& command : commands) {
    if (command->Name == name) {
      return command;
    }
  }

  return nullptr;
}

Query::Variables Query::GetParametersAsVariables() const {
  const auto params{GetParameters()};
  Variables variables{};

  for (auto&& param : params) {
    variables.insert_or_assign(param->Name, CLI::Variable(*param));
  }

  return variables;
}

}  // namespace Litr::Config
