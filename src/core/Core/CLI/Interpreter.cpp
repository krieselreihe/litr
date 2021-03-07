#include "Interpreter.hpp"

#include "Core/CLI/Shell.hpp"

namespace Litr::CLI {

Interpreter::Interpreter(const Ref<ErrorHandler>& errorHandler, const Ref<Instruction>& instruction,
                         const Ref<Config::Loader>& config)
    : m_ErrorHandler(errorHandler), m_Instruction(instruction), m_Config(config), m_Query(m_Config) {
}

void Interpreter::Execute(Interpreter::Callback callback) {
  m_Callback = callback;

  while (m_Offset < m_Instruction->Count()) {
    ExecuteInstruction();
  }
}

void Interpreter::ExecuteInstruction() {
  const auto code{static_cast<CLI::Instruction::Code>(m_Instruction->Read(m_Offset++))};

  switch (code) {
    // @todo: Implement rest
    // NOLINTNEXTLINE
    case Instruction::Code::CLEAR:
      break;
    case Instruction::Code::DEFINE:
      break;
    case Instruction::Code::CONSTANT:
      break;
    case Instruction::Code::BEGIN_SCOPE:
      break;
    case Instruction::Code::EXECUTE:
      CallInstruction();
      break;
    default:
      m_Offset++;
  }
}

void Interpreter::CallInstruction() {
  const std::byte index{m_Instruction->Read(m_Offset)};
  const CLI::Instruction::Value name{m_Instruction->ReadConstant(index)};

  const Ref<Config::Command> command{m_Query.GetCommand(name)};
  CallCommand(name, command);

  m_Offset++;
}

// Ignore recursive call of child commands.
// NOLINTNEXTLINE
void Interpreter::CallCommand(const std::string& name, const Ref<Config::Command>& command) {
  if (command == nullptr) {
    m_ErrorHandler->Push({
        ErrorType::COMMAND_NOT_FOUND,
        fmt::format("Command with the name \"{}\" could not be found.", name)
    });
    return;
  }

  for (auto&& script : command->Script) {
    const CLI::Shell::Result result{CLI::Shell::Exec(script, m_Callback)};
    if (result.Status == ExitStatus::FAILURE) {
      m_ErrorHandler->Push({
          ErrorType::EXECUTION_FAILURE,
          fmt::format("Problem executing the command defined in \"{}\".", name)
      });
    }
  }

  // Execute child commands
  if (!command->ChildCommands.empty()) {
    for (auto&& childCommand : command->ChildCommands) {
      CallCommand(childCommand->Name, childCommand);
    }
  }
}

}  // namespace Litr::CLI
