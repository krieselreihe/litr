#include "Interpreter.hpp"

#include "Core/CLI/Shell.hpp"
#include "Core/Error/Handler.hpp"

namespace Litr::CLI {

Interpreter::Interpreter(const Ref<Instruction>& instruction, const Ref<Config::Loader>& config)
    : m_Instruction(instruction), m_Config(config), m_Query(m_Config) {
}

void Interpreter::Execute(Interpreter::Callback callback) {
  m_Callback = callback;

  while (m_Offset < m_Instruction->Count()) {
    ExecuteInstruction();
  }
}

Instruction::Value Interpreter::ReadCurrentValue() const {
  const std::byte index{m_Instruction->Read(m_Offset)};
  return m_Instruction->ReadConstant(index);
}

std::vector<Variable> Interpreter::GetCurrentVariables() const {
  std::vector<Variable> variables{};
  for (auto&& scope : m_Scope) {
    for (auto&& variable : scope) {
      variables.push_back(variable);
    }
  }
  return variables;
}

void Interpreter::ExecuteInstruction() {
  const auto code{static_cast<Instruction::Code>(m_Instruction->Read(m_Offset++))};

  switch (code) {
    case Instruction::Code::CLEAR:
      ClearScope();
      break;
    case Instruction::Code::DEFINE:
      DefineVariable();
      break;
    case Instruction::Code::CONSTANT:
      SetConstant();
      break;
    case Instruction::Code::BEGIN_SCOPE:
      BeginScope();
      break;
    case Instruction::Code::EXECUTE:
      CallInstruction();
      break;
    default:
      m_Offset++;
  }
}

void Interpreter::BeginScope() {
  m_Scope.emplace_back(std::vector<Variable>());
  m_Offset++;
}

void Interpreter::ClearScope() {
  m_Scope.pop_back();
}

void Interpreter::DefineVariable() {
  const Instruction::Value name{ReadCurrentValue()};
  m_Scope.back().emplace_back(name);
  m_Offset++;
}

void Interpreter::SetConstant() {
  const Instruction::Value name{ReadCurrentValue()};
  m_Scope.back().back().Value = name;
  m_Offset++;
}

void Interpreter::CallInstruction() {
  const Instruction::Value name{ReadCurrentValue()};
  const Ref<Config::Command> command{m_Query.GetCommand(name)};
  CallCommand(name, command);

  m_Offset++;
}

// Ignore recursive call of child commands.
// NOLINTNEXTLINE
void Interpreter::CallCommand(const std::string& name, const Ref<Config::Command>& command) {
  if (command == nullptr) {
    Error::Handler::Push(Error::CommandNotFoundError(
        fmt::format("Command with the name \"{}\" could not be found.", name)
    ));
    return;
  }

  // const auto variables{GetCurrentVariables()};

  for (auto&& script : command->Script) {
    // @todo: Implement
    // InsertVariables(script, variables);

    const Shell::Result result{Shell::Exec(script, m_Callback)};
    if (result.Status == ExitStatus::FAILURE) {
      Error::Handler::Push(Error::ExecutionFailureError(
          fmt::format("Problem executing the command defined in \"{}\".", name)
      ));
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
