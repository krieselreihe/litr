/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>
#include <memory>

#include "Core/CLI/Instruction.hpp"

namespace litr::debug {

size_t disassemble_instruction(const std::shared_ptr<cli::Instruction>& instruction, size_t offset);
void disassemble(const std::shared_ptr<cli::Instruction>& instruction, const std::string& name);

}  // namespace litr::debug

#if LITR_ENABLE_DISASSEMBLE == 1
#define LITR_DISASSEMBLE_INSTRUCTION(instruction, offset) ::litr::debug::disassemble_instruction(instruction, offset)
#define LITR_DISASSEMBLE(instruction, name) ::litr::debug::disassemble(instruction, name)
#else
#define LITR_DISASSEMBLE_INSTRUCTION(instruction, offset)
#define LITR_DISASSEMBLE(instruction, name)
#endif
