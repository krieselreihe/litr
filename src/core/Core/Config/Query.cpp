#include "Query.hpp"

#include "Core/Utils.hpp"

#include "Core/Debug/Instrumentor.hpp"

namespace Litr::Config {

Query::Query(const Ref<Loader>& config) : m_Config(config) {
}

Ref<Command> Query::GetCommand(const std::string& name) const {
  LITR_PROFILE_FUNCTION();

  Parts names{SplitCommandQuery(name)};
  return GetCommandByPath(names, m_Config->GetCommands());
}

Ref<Parameter> Query::GetParameter(const std::string& name) const {
  LITR_PROFILE_FUNCTION();

  for (auto&& param : m_Config->GetParameters()) {
    if (param->Name == name || param->Shortcut == name) {
      return param;
    }
  }

  return nullptr;
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

}  // namespace Litr::Config
