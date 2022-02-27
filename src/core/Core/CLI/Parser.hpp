#pragma once

#include <string>
#include <deque>

#include "Core/Base.hpp"
#include "Core/CLI/Token.hpp"
#include "Core/CLI/Scanner.hpp"
#include "Core/CLI/Instruction.hpp"

namespace Litr::CLI {

class Parser {
 public:
  Parser(const Ref<Instruction>& instruction, const std::string& source);

  void Advance();
  void Consume(TokenType type, const char* message);
  [[nodiscard]] bool Match(TokenType type) const;
  [[nodiscard]] bool Match(std::initializer_list<TokenType> types) const;
  [[nodiscard]] bool Peak(TokenType type) const;
  std::byte MakeConstant(const Instruction::Value& value);

  void EmitByte(std::byte byte);
  void EmitByte(Instruction::Code code);
  void EmitBytes(Instruction::Code code, std::byte byte);

  void EmitConstant(const Instruction::Value& value);
  void EmitDefinition(const Instruction::Value& value);
  void EmitScope(const Instruction::Value& value);
  void EmitExecution();
  void EmitClear();

  void Arguments();
  void Commands();
  void Parameters();
  void Comma();
  void EndOfString();

  [[nodiscard]] inline bool HasErrors() const { return m_HasError; }

 private:
  void Error(const std::string& message);
  void ErrorAtCurrent(const std::string& message);
  void ErrorAt(Token* token, const std::string& message);

  [[nodiscard]] std::string GetScopePath() const;

  std::string m_Source;
  Scanner m_Scanner;
  const Ref<Instruction>& m_Instruction;

  Token m_Current{};
  Token m_Previous{};
  std::deque<std::string> m_Scope{};

  bool m_HasError{false};
  bool m_PanicMode{false};
};

}  // namespace Litr::CLI
