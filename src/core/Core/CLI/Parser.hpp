/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <deque>
#include <memory>
#include <string>

#include "Core/CLI/Instruction.hpp"
#include "Core/CLI/Scanner.hpp"
#include "Core/CLI/Token.hpp"

namespace Litr::CLI {

class Parser {
 public:
  Parser(const std::shared_ptr<Instruction>& instruction, const std::string& source);

  void advance();
  void consume(TokenType type, const char* message);
  [[nodiscard]] bool match(TokenType type) const;
  [[nodiscard]] bool match(std::initializer_list<TokenType> types) const;
  [[nodiscard]] bool peak(TokenType type) const;
  std::byte make_constant(const Instruction::Value& value);

  void emit_byte(std::byte byte);
  void emit_byte(Instruction::Code code);
  void emit_bytes(Instruction::Code code, std::byte byte);
  void emit_constant(const Instruction::Value& value);
  void emit_definition(const Instruction::Value& value);
  void emit_scope(const Instruction::Value& value);
  void emit_execution();
  void emit_clear();

  void arguments();
  void commands();
  void parameters();
  void comma();
  void end_of_string();

  [[nodiscard]] inline bool has_errors() const {
    return m_has_error;
  }

 private:
  void error(const std::string& message);
  void error_at_current(const std::string& message);
  void error_at(Token* token, const std::string& message);

  [[nodiscard]] std::string scope_path() const;

  std::string m_source;
  Scanner m_scanner;
  const std::shared_ptr<Instruction>& m_instruction;

  Token m_current{};
  Token m_previous{};
  std::deque<std::string> m_scope{};

  bool m_has_error{false};
  bool m_panic_mode{false};
};

}  // namespace Litr::CLI
