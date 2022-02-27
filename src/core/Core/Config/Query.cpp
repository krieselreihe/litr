/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Query.hpp"

#include <algorithm>

#include "Core/Utils.hpp"
#include "Core/Debug/Instrumentor.hpp"
#include "Core/Script/Compiler.hpp"

namespace litr::Config {

Query::Query(const Ref<Loader>& config) : m_Config(config) {}

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

Query::Commands Query::GetCommands(const std::string& name) const {
  LITR_PROFILE_FUNCTION();

  const Ref<Command>& command{GetCommand(name)};

  if (command == nullptr) {
    return {};
  }

  return command->ChildCommands;
}

Query::Parameters Query::GetParameters() const {
  LITR_PROFILE_FUNCTION();

  return m_Config->GetParameters();
}

Query::Parameters Query::GetParameters(const std::string& name) const {
  LITR_PROFILE_FUNCTION();

  const Ref<Command>& command{GetCommand(name)};
  Query::Parameters parameters{};

  if (command == nullptr) {
    return parameters;
  }

  std::vector<std::string> names{GetUsedParameterNames(command)};

  for (auto&& childCommand : command->ChildCommands) {
    const std::vector<std::string> childNames{GetUsedParameterNames(childCommand)};
    names.insert(names.end(), childNames.begin(), childNames.end());
  }

  Utils::Deduplicate(names);

  if (!names.empty()) {
    for (auto&& n : names) {
      parameters.push_back(GetParameter(n));
    }
  }

  return parameters;
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
      // Do nothing. This will lead further down the line to a "child command not found"
      // error, as it should be.
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
  LITR_PROFILE_FUNCTION();

  const Query::Parameters params{GetParameters()};
  Variables variables{};

  for (auto&& param : params) {
    variables.insert_or_assign(param->Name, CLI::Variable(*param));
  }

  return variables;
}

std::vector<std::string> Query::GetUsedParameterNames(const Ref<Command>& command) const {
  LITR_PROFILE_FUNCTION();

  std::vector<std::string> names{};
  size_t index{0};

  for (auto&& script : command->Script) {
    const Variables variables{GetParametersAsVariables()};
    const Script::Compiler compiler{script, command->Locations[index++], variables};
    const std::vector<std::string> usedNames{compiler.GetUsedVariables()};
    names.insert(names.end(), usedNames.begin(), usedNames.end());
  }

  Utils::Deduplicate(names);

  return names;
}

}  // namespace litr::Config
