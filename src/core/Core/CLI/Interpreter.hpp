#pragma once

#include <functional>
#include <deque>
#include <utility>
#include <string>
#include <variant>
#include <unordered_map>

#include "Core/CLI/Instruction.hpp"
#include "Core/Config/Loader.hpp"
#include "Core/Config/Query.hpp"
#include "Core/Config/Location.hpp"

namespace Litr::CLI {

struct Variable {
  enum class Type { STRING, BOOLEAN };

  Type Type;
  std::string Name;
  std::variant<bool, std::string> Value{};

  explicit Variable(enum Type type, std::string name) : Type(type), Name(std::move(name)) {
  }
  explicit Variable(std::string name, bool value) : Type(Type::BOOLEAN), Name(std::move(name)), Value(value) {
  }
  explicit Variable(std::string name, std::string value) : Type(Type::STRING), Name(std::move(name)), Value(std::move(value)) {
  }
} __attribute__((aligned(64)));

class Interpreter {
  using Variables = std::unordered_map<std::string, CLI::Variable>;
  using Scripts = std::vector<std::string>;

 public:
  Interpreter(const Ref<Instruction>& instruction, const Ref<Config::Loader>& config);

  void Execute();

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
  static void RunScripts(const Scripts& scripts, const std::string& commandPath, const std::string& dir, bool printResult);

  [[nodiscard]] Scripts ParseScripts(const Ref<Config::Command>& command);
  [[nodiscard]] std::string ParseScript(const std::string& script, const Config::Location& location);

  [[nodiscard]] static enum Variable::Type GetVariableType(const Ref<Config::Parameter>& param);

  static void Print(const std::string& result);

 private:
  const Ref<Instruction>& m_Instruction;
  const Config::Query m_Query;

  size_t m_Offset{0};
  std::string m_CurrentVariableName{};

  // Initialize with empty scope
  std::vector<Variables> m_Scope{Variables()};
};

}  // namespace Litr::CLI
