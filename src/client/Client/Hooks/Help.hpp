#pragma once

#include "Core.hpp"

namespace Litr::Hook {

class Help {
 public:
  explicit Help(const Ref<Config::Loader>& config);
  void Print(const Ref<CLI::Instruction>& instruction) const;

 private:
  void PrintWelcomeMessage() const;
  void PrintUsage() const;
  void PrintCommands() const;
  void PrintCommand(const Ref<Config::Command>& command, const std::string& parentName, size_t depth = 1) const;
  void PrintExample(const Ref<Config::Command>& command) const;
  void PrintOptions() const;
  void PrintParameterOptions(const Ref<Config::Parameter>& param) const;
  void PrintDefaultParameterOption(const Ref<Config::Parameter>& param) const;
  void PrintWithDescription(const std::string& name, const std::string& description, size_t extraPadding = 0) const;

  [[nodiscard]] static std::string GetCommandName(const Ref<CLI::Instruction>& instruction);
  [[nodiscard]] std::string GetCommandArguments(const std::string& name) const;

  [[nodiscard]] size_t GetCommandPadding() const;
  [[nodiscard]] size_t GetCommandPadding(const Config::Query::Commands& commands) const;
  [[nodiscard]] size_t GetParameterPadding() const;

  [[nodiscard]] static bool SortParameterByRequired(const Ref<Config::Parameter>& p1, const Ref<Config::Parameter>& p2);

 private:
  const Config::Query m_Query;
  const Path m_FilePath;

  mutable std::string m_CommandName{};

  const std::string m_ArgumentPlaceholder{"=<option>"};
};

}  // namespace Litr::Hook
