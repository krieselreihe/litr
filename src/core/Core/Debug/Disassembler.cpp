/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Disassembler.hpp"

#include <fmt/format.h>

namespace Litr::Debug {

/** @private */
static size_t SimpleInstruction(const std::string& name, size_t offset) {
  fmt::print("{}\n", name);
  return offset;
}

/** @private */
static size_t ConstantInstruction(const std::string& name, const Ref<CLI::Instruction>& instruction, size_t offset) {
  const std::byte index{instruction->Read(offset)};
  const CLI::Instruction::Value constant{instruction->ReadConstant(index)};

  fmt::print("{:<16} {:4d} '{}'\n", name, index, constant);
  return offset + 1;
}

size_t DisassembleInstruction(const Ref<CLI::Instruction>& instruction, size_t offset) {
  fmt::print("{:04d} ", offset);

  const auto code{static_cast<CLI::Instruction::Code>(instruction->Read(offset++))};

  switch (code) {
    case CLI::Instruction::Code::CONSTANT:
      return ConstantInstruction("CONSTANT", instruction, offset);
    case CLI::Instruction::Code::DEFINE:
      return ConstantInstruction("DEFINE", instruction, offset);
    case CLI::Instruction::Code::BEGIN_SCOPE:
      return ConstantInstruction("BEGIN_SCOPE", instruction, offset);
    case CLI::Instruction::Code::EXECUTE:
      return ConstantInstruction("EXECUTE", instruction, offset);
    case CLI::Instruction::Code::CLEAR:
      return SimpleInstruction("CLEAR", offset);
    default:
      fmt::print("Unknown Instruction::Code {:d}\n", code);
      return offset + 1;
  }
}

void Disassemble(const Ref<CLI::Instruction>& instruction, const std::string& name) {
  fmt::print("=== {} ===\n", name);

  size_t offset{0};

  while (offset < instruction->Count()) {
    offset = DisassembleInstruction(instruction, offset);
  }
}

}  // namespace Litr::Debug
