/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Handler.hpp"

namespace litr::hook {

Handler::Handler(const std::shared_ptr<cli::Instruction>& instruction)
    : m_instruction(instruction) {}

void Handler::add(
    const Code code, const std::vector<Value>& values, const Handler::HookCallback& callback) {
  LITR_PROFILE_FUNCTION();

  for (auto&& value : values) {
    m_hooks.emplace_back(code, value, callback);
  }
}

bool Handler::execute() const {
  LITR_PROFILE_FUNCTION();

  size_t offset{0};

  while (offset < m_instruction->count()) {
    const Code code{m_instruction->read(offset++)};

    for (auto&& hook : m_hooks) {
      if (code != hook.code) {
        continue;
      }
      auto value{m_instruction->read_constant(m_instruction->read(offset))};
      if (value == hook.value) {
        hook.callback(m_instruction);
        return true;
      }
    }

    if (code != Code::CLEAR) {
      ++offset;
    }
  }

  return false;
}

}  // namespace litr::hook
