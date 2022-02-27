/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <vector>
#include <utility>

#include "Core.hpp"

namespace litr::hook {

class Handler {
  using HookCallback = std::function<void(const Ref<cli::Instruction>& instruction)>;
  using Code = cli::Instruction::Code;
  using Value = cli::Instruction::Value;

  struct Hook {
    cli::Instruction::Code code;
    cli::Instruction::Value value;
    HookCallback callback;

    Hook(cli::Instruction::Code code, cli::Instruction::Value value, HookCallback callback)
        : code(code), value(std::move(value)), callback(std::move(callback)) {}
  };

 public:
  explicit Handler(const Ref<cli::Instruction>& instruction);

  void add(Code code, const std::vector<Value>& values, const HookCallback& callback);
  [[nodiscard]] bool execute() const;

 private:
  const Ref<cli::Instruction>& m_instruction;
  std::vector<Hook> m_hooks{};
};

}  // namespace litr::hook
