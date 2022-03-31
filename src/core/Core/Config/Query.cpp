/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Query.hpp"

#include <algorithm>

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Script/Compiler.hpp"
#include "Core/Utils.hpp"

namespace Litr::Config {

Query::Query(const std::shared_ptr<Loader>& config) : m_config(config) {}

std::shared_ptr<Command> Query::get_command(const std::string& name) const {
  LITR_PROFILE_FUNCTION();

  Parts names{split_command_query(name)};
  return get_command_by_path(names, get_commands());
}

std::shared_ptr<Parameter> Query::get_parameter(const std::string& name) const {
  LITR_PROFILE_FUNCTION();

  for (auto&& param : get_parameters()) {
    if (param->name == name || param->shortcut == name) {
      return param;
    }
  }

  return nullptr;
}

Query::Commands Query::get_commands() const {
  LITR_PROFILE_FUNCTION();

  return m_config->get_commands();
}

Query::Commands Query::get_commands(const std::string& name) const {
  LITR_PROFILE_FUNCTION();

  const std::shared_ptr<Command>& command{get_command(name)};

  if (command == nullptr) {
    return {};
  }

  return command->child_commands;
}

Query::Parameters Query::get_parameters() const {
  LITR_PROFILE_FUNCTION();

  return m_config->get_parameters();
}

Query::Parameters Query::get_parameters(const std::string& command_name) const {
  LITR_PROFILE_FUNCTION();

  const std::shared_ptr<Command>& command{get_command(command_name)};
  Query::Parameters parameters{};

  if (command == nullptr) {
    return parameters;
  }

  std::vector<std::string> names{get_used_parameter_names(command)};

  for (auto&& child_command : command->child_commands) {
    const std::vector<std::string> child_names{get_used_parameter_names(child_command)};
    names.insert(names.end(), child_names.begin(), child_names.end());
  }

  Utils::deduplicate(names);

  if (!names.empty()) {
    for (auto&& name : names) {
      parameters.push_back(get_parameter(name));
    }
  }

  return parameters;
}

Query::Parts Query::split_command_query(const std::string& query) {
  LITR_PROFILE_FUNCTION();

  Parts parts{};
  Utils::split_into(query, '.', parts);
  return parts;
}

// NOLINTNEXTLINE(misc-no-recursion)
std::shared_ptr<Command> Query::get_command_by_path(
    Parts& names, const Loader::Commands& commands) {
  LITR_PROFILE_FUNCTION();

  const std::shared_ptr<Command>& command{get_command_by_name(names.front(), commands)};

  if (command == nullptr) {
    return nullptr;
  }

  names.pop_front();

  if (names.empty()) {
    return command;
  }

  if (command->child_commands.empty()) {
    if (!names.empty()) {
      // Do nothing. This will lead further down the line to a "child command not found"
      // error, as it should be.
    }

    return nullptr;
  }

  // NOLINTNEXTLINE(misc-no-recursion)
  return get_command_by_path(names, command->child_commands);
}

std::shared_ptr<Command> Query::get_command_by_name(
    const std::string& name, const Loader::Commands& commands) {
  LITR_PROFILE_FUNCTION();

  for (const std::shared_ptr<Command>& command : commands) {
    if (command->name == name) {
      return command;
    }
  }

  return nullptr;
}

Query::Variables Query::get_parameters_as_variables() const {
  LITR_PROFILE_FUNCTION();

  const Query::Parameters params{get_parameters()};
  Variables variables{};

  for (auto&& param : params) {
    variables.insert_or_assign(param->name, CLI::Variable(*param));
  }

  return variables;
}

std::vector<std::string> Query::get_used_parameter_names(
    const std::shared_ptr<Command>& command) const {
  LITR_PROFILE_FUNCTION();

  std::vector<std::string> names{};
  size_t index{0};

  for (auto&& script : command->script) {
    const Variables variables{get_parameters_as_variables()};
    const Script::Compiler compiler{script, command->Locations[index++], variables};
    const std::vector<std::string> used_names{compiler.get_used_variables()};
    names.insert(names.end(), used_names.begin(), used_names.end());
  }

  Utils::deduplicate(names);

  return names;
}

}  // namespace Litr::Config
