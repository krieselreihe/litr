/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <memory>
#include <string>

#include "Core/CLI/Instruction.hpp"

namespace Litr::Debug {

size_t disassemble_instruction(const std::shared_ptr<CLI::Instruction>& instruction, size_t offset);
void disassemble(const std::shared_ptr<CLI::Instruction>& instruction, const std::string& name);

}  // namespace Litr::Debug

#if LITR_ENABLE_DISASSEMBLE == 1
#define LITR_DISASSEMBLE_INSTRUCTION(instruction, offset) \
  ::Litr::Debug::disassemble_instruction(instruction, offset)
#define LITR_DISASSEMBLE(instruction, name) ::Litr::Debug::disassemble(instruction, name)
#else
#define LITR_DISASSEMBLE_INSTRUCTION(instruction, offset)
#define LITR_DISASSEMBLE(instruction, name)
#endif
