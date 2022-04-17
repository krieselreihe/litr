/*
 * Copyright (c) 2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <memory>
#include <string>

#include "Core.hpp"

namespace Litr::Hook {

class HelpStrategy {
 public:
  HelpStrategy(const std::shared_ptr<Config::Loader>& config,
      const std::shared_ptr<CLI::Instruction>& instruction)
      : m_config(config),
        m_instruction(instruction){};

  HelpStrategy(const HelpStrategy&) = default;
  HelpStrategy(HelpStrategy&&) = default;
  HelpStrategy& operator=(const HelpStrategy&) = delete;
  HelpStrategy& operator=(HelpStrategy&&) = delete;

  virtual ~HelpStrategy() = default;

  [[nodiscard]] virtual std::string usage() const = 0;
  [[nodiscard]] virtual Litr::Config::Commands commands() const = 0;
  [[nodiscard]] virtual Litr::Config::Parameters parameters() const = 0;

 protected:
  const std::shared_ptr<Config::Loader>& m_config;
  const std::shared_ptr<CLI::Instruction>& m_instruction;
};

}  // namespace Litr::Hook
