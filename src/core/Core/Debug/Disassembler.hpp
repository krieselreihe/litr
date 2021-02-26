#pragma once

#include "Core/Base.hpp"
#include "Core/CLI/Instruction.hpp"

namespace Litr::Debug {

size_t DisassembleInstruction(const Ref<Instruction>& instruction, size_t offset);
void Disassemble(const Ref<Instruction>& instruction, const std::string& name);

}  // namespace Litr::Debug

#if LITR_ENABLE_DISASSEMBLE == 1
#define LITR_DISASSEMBLE_INSTRUCTION(instruction, offset) ::Litr::Debug::DisassembleInstruction(instruction, offset)
#define LITR_DISASSEMBLE(instruction, name) ::Litr::Debug::Disassemble(instruction, name)
#else
#define LITR_DISASSEMBLE_INSTRUCTION(instruction, offset)
#define LITR_DISASSEMBLE(instruction, name)
#endif
