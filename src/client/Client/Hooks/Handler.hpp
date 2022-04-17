/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "Core.hpp"

namespace Litr::Hook {

class Handler {
  using HookCallback = std::function<void()>;
  using Code = CLI::Instruction::Code;
  using Value = CLI::Instruction::Value;

  struct Hook {
    CLI::Instruction::Code code;
    CLI::Instruction::Value value;
    HookCallback callback;

    Hook(CLI::Instruction::Code code, CLI::Instruction::Value value, HookCallback callback)
        : code(code),
          value(std::move(value)),
          callback(std::move(callback)) {}
  };

 public:
  explicit Handler(const std::shared_ptr<CLI::Instruction>& instruction);

  void add(Code code, const std::vector<Value>& values, const HookCallback& callback);
  [[nodiscard]] bool execute() const;

 private:
  const std::shared_ptr<CLI::Instruction>& m_instruction;
  std::vector<Hook> m_hooks{};
};

}  // namespace Litr::Hook
