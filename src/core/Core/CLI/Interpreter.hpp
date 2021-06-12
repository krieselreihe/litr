#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <utility>

#include "Core/CLI/Instruction.hpp"
#include "Core/CLI/Variable.hpp"
#include "Core/Config/Loader.hpp"
#include "Core/Config/Location.hpp"
#include "Core/Config/Query.hpp"
#include "Core/Error/Handler.hpp"

namespace Litr::CLI {

class Interpreter {
  using Variables = std::unordered_map<std::string, CLI::Variable>;
  using Scripts = std::vector<std::string>;
  using HookCallback = std::function<void(const Ref<Instruction>& instruction)>;

  struct Hook {
    Instruction::Code Code;
    Instruction::Value Value;
    HookCallback Callback;

    Hook(Instruction::Code code, Instruction::Value value, HookCallback callback)
        : Code(code), Value(std::move(value)), Callback(std::move(callback)) {}
  };

 public:
  Interpreter(const Ref<Instruction>& instruction, const Ref<Config::Loader>& config);

  void Execute();
  void AddHook(Instruction::Code code, const Instruction::Value& value, const HookCallback& callback);
  void AddHook(Instruction::Code code, const std::vector<Instruction::Value>& values, const HookCallback& callback);

 private:
  [[nodiscard]] Instruction::Value ReadCurrentValue() const;
  [[nodiscard]] Variables GetScopeVariables() const;
  void DefineDefaultVariables(const Ref<Config::Loader>& config);

  void ExecuteInstruction();

  void BeginScope();
  void ClearScope();
  void DefineVariable();
  void SetConstant();
  void CallInstruction();

  void CallCommand(const Ref<Config::Command>& command, const std::string& scope = "");
  void CallChildCommands(const Ref<Config::Command>& command, const std::string& scope);
  void RunScripts(const Scripts& scripts, const std::string& commandPath, const std::string& dir, bool printResult);

  [[nodiscard]] Scripts ParseScripts(const Ref<Config::Command>& command);
  [[nodiscard]] std::string ParseScript(const std::string& script, const Config::Location& location);

  [[nodiscard]] static enum Variable::Type GetVariableType(const Ref<Config::Parameter>& param);
  [[nodiscard]] bool HookExecuted() const;

  void ValidateRequiredParameters(const Ref<Config::Command>& command);
  [[nodiscard]] bool IsVariableDefined(const std::string& name) const;
  void HandleError(const Error::BaseError& error);

  static void Print(const std::string& message);

 private:
  const Ref<Instruction>& m_Instruction;
  const Config::Query m_Query;

  size_t m_Offset{0};
  std::string m_CurrentVariableName{};
  bool m_StopExecution{false};

  std::vector<Hook> m_Hooks{};

  // Initialize with empty scope
  std::vector<Variables> m_Scope{Variables()};
};

}  // namespace Litr::CLI
