#pragma once

#include <functional>
#include <deque>
#include <utility>

#include "Core/CLI/Instruction.hpp"
#include "Core/Config/Loader.hpp"
#include "Core/Config/Query.hpp"

namespace Litr::CLI {

struct Variable {
  std::string Name;
  std::string Value{};

  explicit Variable(std::string name) : Name(std::move(name)) {
  }
};

class Interpreter {
 public:
  Interpreter(const Ref<Instruction>& instruction, const Ref<Config::Loader>& config);

  void Execute();

 private:
  [[nodiscard]] Instruction::Value ReadCurrentValue() const;
  [[nodiscard]] std::vector<Variable> GetCurrentVariables() const;

  void ExecuteInstruction();

  void BeginScope();
  void ClearScope();
  void DefineVariable();
  void SetConstant();
  void CallInstruction();
  void CallCommand(const std::string& name, const Ref<Config::Command>& command);
  void CallChildCommands(const Ref<Config::Command>& command);

  static void Print(const std::string& result);

 private:
  const Ref<Instruction>& m_Instruction;
  const Ref<Config::Loader>& m_Config;
  const Config::Query m_Query;

  size_t m_Offset{0};

  // Initialize with empty scope
  std::vector<std::vector<Variable>> m_Scope{std::vector<Variable>()};
};

}  // namespace Litr::CLI
