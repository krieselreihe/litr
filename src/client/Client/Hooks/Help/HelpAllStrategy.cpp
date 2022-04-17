/*
 * Copyright (c) 2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "HelpAllStrategy.hpp"

namespace Litr::Hook {

HelpAllStrategy::HelpAllStrategy(const std::shared_ptr<Config::Loader>& config,
    const std::shared_ptr<CLI::Instruction>& instruction)
    : HelpStrategy(config, instruction),
      m_query(std::make_unique<Config::Query>(config)) {}

std::string HelpAllStrategy::usage() const {
  LITR_PROFILE_FUNCTION();

  return "Usage: litr command [options]\n\nCommands:\n";
}

Litr::Config::Commands HelpAllStrategy::commands() const {
  LITR_PROFILE_FUNCTION();

  return m_query->get_commands();
}

Litr::Config::Parameters HelpAllStrategy::parameters() const {
  LITR_PROFILE_FUNCTION();

  return m_query->get_parameters();
}

}  // namespace Litr::Hook
