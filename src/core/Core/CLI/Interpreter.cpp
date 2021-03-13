#include "Interpreter.hpp"

#include <algorithm>
#include <fmt/printf.h>

#include "Core/CLI/Shell.hpp"
#include "Core/Error/Handler.hpp"
#include "Core/Script/Parser.hpp"

namespace Litr::CLI {

static void CommandPathToHumanReadable(std::string& path) {
  std::replace(path.begin(), path.end(), '.', ' ');
}

Interpreter::Interpreter(const Ref<Instruction>& instruction, const Ref<Config::Loader>& config)
    : m_Instruction(instruction), m_Config(config), m_Query(m_Config) {
}

void Interpreter::Execute() {
  while (m_Offset < m_Instruction->Count()) {
    if (Error::Handler::HasErrors()) return;
    ExecuteInstruction();
  }
}

Instruction::Value Interpreter::ReadCurrentValue() const {
  const std::byte index{m_Instruction->Read(m_Offset)};
  return m_Instruction->ReadConstant(index);
}

std::vector<Variable> Interpreter::GetScopeVariables() const {
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

  if (command == nullptr) {
    Error::Handler::Push(Error::CommandNotFoundError(
        fmt::format("Command \"{}\" could not be found.", name)
    ));
    return;
  }

  CallCommand(name, command);
  m_Offset++;
}

// Ignore recursive call of child commands.
// NOLINTNEXTLINE
void Interpreter::CallCommand(const std::string& name, const Ref<Config::Command>& command, const std::string& scope) {
  std::string commandPath{scope + name};
  CommandPathToHumanReadable(commandPath);

  const std::vector<std::string> scripts{ParseScripts(command)};
  if (Error::Handler::HasErrors()) return;

  for (auto&& script : scripts) {
    Shell::Result result;
    if (command->Output == Config::Command::Output::SILENT) {
      result = Shell::Exec(script);
    } else {
      result = Shell::Exec(script, Print);
    }

    if (result.Status == ExitStatus::FAILURE) {
      Error::Handler::Push(Error::ExecutionFailureError(
          fmt::format("Problem executing the command defined in \"{}\".", commandPath)
      ));
      return;
    }
  }

  CallChildCommands(command, commandPath.append(" "));
}

// Ignore recursive call of child commands.
// NOLINTNEXTLINE
void Interpreter::CallChildCommands(const Ref<Config::Command>& command, const std::string& scope) {
  if (!command->ChildCommands.empty()) {
    for (auto&& childCommand : command->ChildCommands) {
      if (Error::Handler::HasErrors()) return;
      CallCommand(childCommand->Name, childCommand, scope);
    }
  }
}

std::vector<std::string> Interpreter::ParseScripts(const Ref<Config::Command>& command) {
  size_t location{0};
  std::vector<std::string> scripts{};

  for (auto&& script : command->Script) {
    const std::string parsedScript{ParseScript(script, command->Locations[location])};
    if (Error::Handler::HasErrors()) break;
    scripts.push_back(parsedScript);
    location++;
  }

  return scripts;
}

std::string Interpreter::ParseScript(const std::string& script, const Config::Location& location) {
  std::vector<Variable> variables{GetScopeVariables()};
  Script::Parser parser{script, location, variables};
  return parser.GetScript();
}

void Interpreter::Print(const std::string& result) {
  fmt::print("{}", result);
}

}  // namespace Litr::CLI
