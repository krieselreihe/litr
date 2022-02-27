/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <vector>
#include <utility>

#include "Core.hpp"

namespace Litr::Hook {

class Handler {
  using HookCallback = std::function<void(const Ref<CLI::Instruction>& instruction)>;
  using Code = CLI::Instruction::Code;
  using Value = CLI::Instruction::Value;

  struct Hook {
    CLI::Instruction::Code Code;
    CLI::Instruction::Value Value;
    HookCallback Callback;

    Hook(CLI::Instruction::Code code, CLI::Instruction::Value value, HookCallback callback)
        : Code(code), Value(std::move(value)), Callback(std::move(callback)) {}
  };

 public:
  explicit Handler(const Ref<CLI::Instruction>& instruction);
  void Add(Code code, const std::vector<Value>& values, const HookCallback& callback);
  [[nodiscard]] bool Execute() const;

 private:
  const Ref<CLI::Instruction>& m_Instruction;
  std::vector<Hook> m_Hooks{};
};

}  // namespace Litr::Hook
