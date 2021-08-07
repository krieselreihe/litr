#include "Handler.hpp"

namespace Litr::Hook {

Handler::Handler(const Ref<CLI::Instruction>& instruction) : m_Instruction(instruction) {}

void Handler::Add(Code code, const std::vector<Value>& values,
    const Handler::HookCallback& callback) {
  LITR_PROFILE_FUNCTION();

  for (auto&& value : values) {
    m_Hooks.emplace_back(code, value, callback);
  }
}

bool Handler::Execute() const {
  LITR_PROFILE_FUNCTION();

  size_t offset{0};

  while (offset < m_Instruction->Count()) {
    const auto code{static_cast<Code>(m_Instruction->Read(offset++))};

    for (auto&& hook : m_Hooks) {
      if (code != hook.Code) continue;
      auto value{m_Instruction->ReadConstant(m_Instruction->Read(offset))};
      if (value == hook.Value) {
        hook.Callback(m_Instruction);
        return true;
      }
    }

    if (code != Code::CLEAR) {
      offset++;
    }
  }

  return false;
}

}  // namespace Litr::Hook
