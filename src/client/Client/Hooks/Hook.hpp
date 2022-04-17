/*
 * Copyright (c) 2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <memory>
#include <string>

#include "Core.hpp"

namespace Litr::Hook {

class Hook {
 public:
  Hook(const std::shared_ptr<Config::Loader>& config,
      const std::shared_ptr<CLI::Instruction>& instruction)
      : m_config(config),
        m_instruction(instruction){};

  Hook(const Hook&) = default;
  Hook(Hook&&) = default;
  Hook& operator=(const Hook&) = delete;
  Hook& operator=(Hook&&) = delete;

  virtual ~Hook() = default;

  [[nodiscard]] virtual std::string read() = 0;

 protected:
  const std::shared_ptr<Config::Loader>& m_config;
  const std::shared_ptr<CLI::Instruction>& m_instruction;
};

}  // namespace Litr::Hook
