/*
 * Copyright (c) 2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <memory>
#include <string>

#include "Client/Hooks/Help/HelpStrategy.hpp"
#include "Core.hpp"

namespace Litr::Hook {

class HelpCommandStrategy : public HelpStrategy {
 public:
  HelpCommandStrategy(const std::shared_ptr<Config::Loader>& config,
      const std::shared_ptr<CLI::Instruction>& instruction);

  [[nodiscard]] std::string usage() const override;
  [[nodiscard]] Litr::Config::Commands commands() const override;
  [[nodiscard]] Litr::Config::Parameters parameters() const override;

 private:
  std::string m_command_name;
  const std::unique_ptr<Config::Query> m_query;
};

}  // namespace Litr::Hook
