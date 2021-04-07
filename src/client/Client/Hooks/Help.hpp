#pragma once

#include "Core.hpp"

namespace Litr {

class Help {
 public:
  explicit Help(const Ref<Config::Loader>& config);
  void Print(const Ref<CLI::Instruction>& instruction) const;

 private:
  void PrintWelcomeMessage() const;
  void PrintUsage() const;
  void PrintCommands() const;
  void PrintOptions() const;
  void PrintOptions(const Ref<Config::Parameter>& param) const;
  void PrintDefaultOption(const Ref<Config::Parameter>& param) const;
  void PrintWithDescription(const std::string& name, const std::string& description, size_t extraPadding = 0) const;

  [[nodiscard]] std::string GetCommandArguments(const std::string& name) const;

  void SetGlobalPadding();
  [[nodiscard]] size_t GetCommandPadding() const;
  [[nodiscard]] size_t GetParameterPadding() const;

 private:
  const Config::Query m_Query;
  const Path m_FilePath;

  size_t m_Padding{0};
  size_t m_CommandPadding{0};

  const std::string m_ArgumentPlaceholder{"=<option>"};
};

}  // namespace Litr
