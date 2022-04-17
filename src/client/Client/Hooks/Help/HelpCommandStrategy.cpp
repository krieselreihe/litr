/*
 * Copyright (c) 2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "HelpCommandStrategy.hpp"

namespace Litr::Hook {

HelpCommandStrategy::HelpCommandStrategy(const std::shared_ptr<Config::Loader>& config,
    const std::shared_ptr<CLI::Instruction>& instruction)
    : HelpStrategy(config, instruction),
      m_query(std::make_unique<Config::Query>(config)) {
  LITR_PROFILE_FUNCTION();

  const CLI::Interpreter interpreter{m_instruction, m_config};
  m_command_name = interpreter.command_name_occurrence({"h", "help"});
}

std::string HelpCommandStrategy::usage() const {
  LITR_PROFILE_FUNCTION();

  const std::string command_name{CLI::Interpreter::command_path_to_human_readable(m_command_name)};

  std::string usage_text{fmt::format("Usage: litr {} [options]", command_name)};

  const std::string description{m_query->get_command(m_command_name)->description};
  if (!description.empty()) {
    usage_text += fmt::format("\n  {}", m_query->get_command(m_command_name)->description);
  }

  usage_text += "\n\n";

  return usage_text;
}

Litr::Config::Commands HelpCommandStrategy::commands() const {
  LITR_PROFILE_FUNCTION();

  return m_query->get_commands(m_command_name);
}

Litr::Config::Parameters HelpCommandStrategy::parameters() const {
  LITR_PROFILE_FUNCTION();

  return m_query->get_parameters(m_command_name);
}

}  // namespace Litr::Hook
