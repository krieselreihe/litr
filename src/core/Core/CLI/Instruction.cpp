/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Instruction.hpp"

#include <utility>

#include "Core/Debug/Instrumentor.hpp"

namespace Litr::CLI {

Instruction::Instruction(std::vector<std::byte> data) : m_byte_code(std::move(data)) {}

void Instruction::write(Instruction::Code code) {
  write(static_cast<std::byte>(code));
}

void Instruction::write(const std::byte byte) {
  LITR_PROFILE_FUNCTION();

  m_byte_code.push_back(byte);
}

std::byte Instruction::read(const size_t offset) const {
  LITR_PROFILE_FUNCTION();

  return m_byte_code[offset];
}

size_t Instruction::count() const {
  LITR_PROFILE_FUNCTION();

  return m_byte_code.size();
}

std::byte Instruction::write_constant(const Instruction::Value& value) {
  LITR_PROFILE_FUNCTION();

  m_constants.push_back(value);
  return static_cast<std::byte>(m_constants.size() - 1);
}

Instruction::Value Instruction::read_constant(const std::byte index) const {
  LITR_PROFILE_FUNCTION();

  return m_constants[static_cast<size_t>(index)];
}

}  // namespace Litr::CLI
