/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Disassembler.hpp"

#include <fmt/format.h>

namespace Litr::Debug {

/** @private */
static size_t simple_instruction(const std::string& name, const size_t offset) {
  fmt::print("{}\n", name);
  return offset;
}

/** @private */
static size_t constant_instruction(const std::string& name,
    const std::shared_ptr<CLI::Instruction>& instruction,
    const size_t offset) {
  const std::byte index{instruction->read(offset)};
  const CLI::Instruction::Value constant{instruction->read_constant(index)};

  fmt::print("{:<16} {:4d} '{}'\n", name, index, constant);
  return offset + 1;
}

size_t disassemble_instruction(
    const std::shared_ptr<CLI::Instruction>& instruction, size_t offset) {
  fmt::print("{:04d} ", offset);

  const CLI::Instruction::Code code{instruction->read(offset++)};

  switch (code) {
    case CLI::Instruction::Code::CONSTANT:
      return constant_instruction("CONSTANT", instruction, offset);
    case CLI::Instruction::Code::DEFINE:
      return constant_instruction("DEFINE", instruction, offset);
    case CLI::Instruction::Code::BEGIN_SCOPE:
      return constant_instruction("BEGIN_SCOPE", instruction, offset);
    case CLI::Instruction::Code::EXECUTE:
      return constant_instruction("EXECUTE", instruction, offset);
    case CLI::Instruction::Code::CLEAR:
      return simple_instruction("CLEAR", offset);
    default:
      fmt::print("Unknown Instruction::Code {:d}\n", code);
      return offset + 1;
  }
}

void disassemble(const std::shared_ptr<CLI::Instruction>& instruction, const std::string& name) {
  fmt::print("=== {} ===\n", name);

  size_t offset{0};

  while (offset < instruction->count()) {
    offset = disassemble_instruction(instruction, offset);
  }
}

}  // namespace Litr::Debug
