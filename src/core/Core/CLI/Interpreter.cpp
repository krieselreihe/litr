#include "Interpreter.hpp"

#include <algorithm>

#include "Core/Debug/Instrumentor.hpp"
#include "Core/CLI/Shell.hpp"
#include "Core/Script/Compiler.hpp"
#include "Core/Utils.hpp"

namespace Litr::CLI {

static void CommandPathToHumanReadable(std::string& path) {
  LITR_PROFILE_FUNCTION();

  std::replace(path.begin(), path.end(), '.', ' ');
}

Interpreter::Interpreter(const Ref<Instruction>& instruction, const Ref<Config::Loader>& config)
    : m_Instruction(instruction), m_Query(config) {
  DefineDefaultVariables(config);
}

void Interpreter::Execute() {
  LITR_PROFILE_FUNCTION();

  m_Offset = 0;

  if (HookExecuted()) return;

  while (m_Offset < m_Instruction->Count()) {
    if (m_StopExecution) return;
    ExecuteInstruction();
  }
}

void Interpreter::AddHook(Instruction::Code code, const Instruction::Value& value, const Interpreter::HookCallback& callback) {
  LITR_PROFILE_FUNCTION();

  m_Hooks.emplace_back(code, value, callback);
}

void Interpreter::AddHook(Instruction::Code code, const std::vector<Instruction::Value>& values, const Interpreter::HookCallback& callback) {
  LITR_PROFILE_FUNCTION();

  for (auto&& value : values) {
    AddHook(code, value, callback);
  }
}

Instruction::Value Interpreter::ReadCurrentValue() const {
  LITR_PROFILE_FUNCTION();

  const std::byte index{m_Instruction->Read(m_Offset)};
  return m_Instruction->ReadConstant(index);
}

Interpreter::Variables Interpreter::GetScopeVariables() const {
  LITR_PROFILE_FUNCTION();

  Variables variables{};

  for (auto&& scope : m_Scope) {
    for (auto&& variable : scope) {
      variables.insert_or_assign(variable.first, variable.second);
    }
  }

  return variables;
}

void Interpreter::DefineDefaultVariables(const Ref<Config::Loader>& config) {
  LITR_PROFILE_FUNCTION();

  const auto params{config->GetParameters()};
  for (auto&& param : params) {
    switch (param->Type) {
      case Config::Parameter::Type::BOOLEAN: {
        Variable variable{param->Name, false};
        m_Scope.back().insert_or_assign(variable.Name, variable);
        break;
      }
      case Config::Parameter::Type::STRING:
      case Config::Parameter::Type::ARRAY: {
        if (!param->Default.empty()) {
          Variable variable{param->Name, param->Default};
          m_Scope.back().insert_or_assign(variable.Name, variable);
        }
        break;
      }
    }
  }
}

void Interpreter::ExecuteInstruction() {
  LITR_PROFILE_FUNCTION();

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
  LITR_PROFILE_FUNCTION();

  m_Scope.emplace_back(Variables());
  m_Offset++;
}

void Interpreter::ClearScope() {
  LITR_PROFILE_FUNCTION();

  m_Scope.pop_back();
}

void Interpreter::DefineVariable() {
  LITR_PROFILE_FUNCTION();

  const Instruction::Value name{ReadCurrentValue()};
  const Ref<Config::Parameter>& param{m_Query.GetParameter(name)};

  if (param == nullptr) {
    HandleError(Error::CommandNotFoundError(
        fmt::format("Parameter with the name \"{}\" is not defined.\n  Run `litr --help` to see a list available options.", name)
    ));
    return;
  }

  Variable variable{GetVariableType(param), param->Name};

  switch (param->Type) {
    case Config::Parameter::Type::BOOLEAN: {
      variable.Value = true;
      break;
    }
    case Config::Parameter::Type::STRING:
    case Config::Parameter::Type::ARRAY: {
      if (!param->Default.empty()) {
        variable.Value = param->Default;
      }
      break;
    }
  }

  m_CurrentVariableName = variable.Name;
  m_Scope.back().insert_or_assign(variable.Name, variable);

  m_Offset++;
}

void Interpreter::SetConstant() {
  LITR_PROFILE_FUNCTION();

  const Instruction::Value value{ReadCurrentValue()};
  CLI::Variable& variable{m_Scope.back().at(m_CurrentVariableName)};
  const auto param{m_Query.GetParameter(variable.Name)};

  switch (param->Type) {
    case Config::Parameter::Type::STRING: {
      variable.Value = value;
      break;
    }
    case Config::Parameter::Type::ARRAY: {
      const auto& args{param->TypeArguments};
      if (std::find(args.begin(), args.end(), value) == args.end()) {
        std::string options{"Available options are:"};
        for (auto&& option : args) {
          options.append(fmt::format(" \"{}\",", option));
        }

        HandleError(Error::CommandNotFoundError(
            fmt::format(
                "Parameter value \"{}\" is no valid option for \"{}\".\n  {}",
                value, param->Name, Utils::TrimRight(options, ','))
        ));
        return;
      }
      variable.Value = value;
      break;
    }
    case Config::Parameter::Type::BOOLEAN:
      if (value == "false") {
        variable.Value = false;
      } else if (value == "true") {
        variable.Value = true;
      } else {
        HandleError(Error::CommandNotFoundError(
            fmt::format(
                "Parameter value \"{}\" is not valid for boolean option \"{}\".\n  Please use \"false\", \"true\" or no value for true as well.",
                value, param->Name)
        ));
      }
      break;
  }

  m_Scope.back().insert_or_assign(variable.Name, variable);
  m_Offset++;
}

void Interpreter::CallInstruction() {
  LITR_PROFILE_FUNCTION();

  const Instruction::Value name{ReadCurrentValue()};
  const Ref<Config::Command> command{m_Query.GetCommand(name)};

  if (command == nullptr) {
    HandleError(Error::CommandNotFoundError(
        fmt::format(
            "Command \"{}\" could not be found.\n  Run `litr --help` to see a list of commands.",
            name)
    ));
    return;
  }

  CallCommand(command);
  m_Offset++;
}

// Ignore recursive call of child commands.
// NOLINTNEXTLINE
void Interpreter::CallCommand(const Ref<Config::Command>& command, const std::string& scope) {
  LITR_PROFILE_FUNCTION();

  std::string commandPath{scope + command->Name};
  const bool printResult{command->Output == Config::Command::Output::SILENT};
  const Scripts scripts{ParseScripts(command)};

  if (m_StopExecution) return;

  CommandPathToHumanReadable(commandPath);

  if (command->Directory.empty()) {
    RunScripts(scripts, commandPath, "", printResult);
  } else {
    for (auto&& dir : command->Directory) {
      RunScripts(scripts, commandPath, dir, printResult);
    }
  }

  if (m_StopExecution) return;

  CallChildCommands(command, commandPath.append(" "));
}

// Ignore recursive call of child commands.
// NOLINTNEXTLINE
void Interpreter::CallChildCommands(const Ref<Config::Command>& command, const std::string& scope) {
  LITR_PROFILE_FUNCTION();

  if (!command->ChildCommands.empty()) {
    for (auto&& childCommand : command->ChildCommands) {
      if (m_StopExecution) return;
      CallCommand(childCommand, scope);
    }
  }
}

void Interpreter::RunScripts(const Scripts& scripts, const std::string& commandPath, const std::string& dir, bool printResult) {
  LITR_PROFILE_FUNCTION();

  Path path{dir};

  for (auto&& script : scripts) {
    Shell::Result result{printResult ? Shell::Exec(script, path) : Shell::Exec(script, path, Print)};

    if (result.Status == ExitStatus::FAILURE) {
      HandleError(Error::ExecutionFailureError(
          fmt::format("Problem executing the command defined in \"{}\".", commandPath)
      ));
      return;
    }
  }
}

Interpreter::Scripts Interpreter::ParseScripts(const Ref<Config::Command>& command) {
  LITR_PROFILE_FUNCTION();

  size_t location{0};
  Scripts scripts{};

  for (auto&& script : command->Script) {
    const std::string parsedScript{ParseScript(script, command->Locations[location])};
    if (m_StopExecution) break;
    scripts.push_back(parsedScript);
    location++;
  }

  return scripts;
}

std::string Interpreter::ParseScript(const std::string& script, const Config::Location& location) {
  LITR_PROFILE_FUNCTION();

  const Variables variables{GetScopeVariables()};
  Script::Compiler parser{script, location, variables};

  if (Error::Handler::HasErrors()) {
    m_StopExecution = true;
  }

  return parser.GetScript();
}

enum Variable::Type Interpreter::GetVariableType(const Ref<Config::Parameter>& param) {
  LITR_PROFILE_FUNCTION();

  switch (param->Type) {
    case Config::Parameter::Type::BOOLEAN: {
      return Variable::Type::BOOLEAN;
    }
    case Config::Parameter::Type::STRING:
    case Config::Parameter::Type::ARRAY: {
      return Variable::Type::STRING;
    }
  }
}

bool Interpreter::HookExecuted() const {
  LITR_PROFILE_FUNCTION();

  size_t offset{0};

  while (offset < m_Instruction->Count()) {
    const auto code{static_cast<Instruction::Code>(m_Instruction->Read(offset++))};

    for (auto&& hook : m_Hooks) {
      if (code != hook.Code) continue;
      auto value{m_Instruction->ReadConstant(m_Instruction->Read(offset))};
      if (value == hook.Value) {
        hook.Callback(m_Instruction);
        return true;
      }
    }

    if (code != Instruction::Code::CLEAR) {
      offset++;
    }
  }

  return false;
}

void Interpreter::HandleError(const Error::BaseError& error) {
  LITR_PROFILE_FUNCTION();

  m_StopExecution = true;
  Error::Handler::Push(error);
}

void Interpreter::Print(const std::string& message) {
  LITR_PROFILE_FUNCTION();

  fmt::print("{}", message);
}

}  // namespace Litr::CLI
