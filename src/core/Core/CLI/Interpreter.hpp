#pragma once

#include <functional>

#include "Core/CLI/Instruction.hpp"
#include "Core/Config/Loader.hpp"
#include "Core/Config/Query.hpp"

// static void NoOp([[maybe_unused]] const std::string& result) {}

namespace Litr::CLI {

class Interpreter {
 public:
  Interpreter(const Ref<ErrorHandler>& errorHandler, const Ref<Instruction>& instruction,
              const Ref<Config::Loader>& config);

  using Callback = void (*)(const std::string& result);
  void Execute(Interpreter::Callback callback);

 private:
  void ExecuteInstruction();
  void CallInstruction();
  void CallCommand(const std::string& name, const Ref<Config::Command>& command);

 private:
  const Ref<ErrorHandler>& m_ErrorHandler;
  const Ref<Instruction>& m_Instruction;
  const Ref<Config::Loader>& m_Config;
  const Config::Query m_Query;

  size_t m_Offset{0};
  Interpreter::Callback m_Callback{nullptr};
};

}  // namespace Litr::CLI
