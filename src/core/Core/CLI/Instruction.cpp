#include "Instruction.hpp"

#include "Core/Debug/Instrumentor.hpp"

namespace Litr::CLI {

Instruction::Instruction(std::vector<std::byte> data) : m_ByteCode(std::move(data)) {
}

void Instruction::Write(Instruction::Code code) {
  Write(static_cast<std::byte>(code));
}

void Instruction::Write(std::byte byte) {
  LITR_PROFILE_FUNCTION();

  m_ByteCode.push_back(byte);
}

std::byte Instruction::Read(size_t offset) const {
  LITR_PROFILE_FUNCTION();

  return m_ByteCode[offset];
}

size_t Instruction::Count() const {
  LITR_PROFILE_FUNCTION();

  return m_ByteCode.size();
}

size_t Instruction::Size() const {
  LITR_PROFILE_FUNCTION();

  return sizeof(Instruction::Code) * m_ByteCode.size();
}

std::byte Instruction::WriteConstant(const Instruction::Value& value) {
  LITR_PROFILE_FUNCTION();

  m_Constants.push_back(value);
  return static_cast<std::byte>(m_Constants.size() - 1);
}

Instruction::Value Instruction::ReadConstant(std::byte index) const {
  LITR_PROFILE_FUNCTION();

  return m_Constants[static_cast<size_t>(index)];
}

}  // namespace Litr::CLI
