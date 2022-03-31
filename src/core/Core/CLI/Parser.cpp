/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Parser.hpp"

#include "Core/CLI/Scanner.hpp"
#include "Core/Debug/Disassembler.hpp"
#include "Core/Debug/Instrumentor.hpp"
#include "Core/Error/Handler.hpp"
#include "Core/Utils.hpp"

namespace Litr::CLI {

Parser::Parser(const std::shared_ptr<Instruction>& instruction, const std::string& source)
    : m_source(source),
      m_scanner(source.c_str()),
      m_instruction(instruction) {
  LITR_PROFILE_FUNCTION();

  advance();
  arguments();
  end_of_string();

  LITR_DISASSEMBLE(m_instruction, "CLI::Parser");
}

void Parser::advance() {
  LITR_PROFILE_FUNCTION();

  m_previous = m_current;

  while (true) {
    m_current = m_scanner.scan_token();

    if (m_current.type != TokenType::ERROR) {
      break;
    }

    error_at_current(m_current.start);
  }
}

void Parser::consume(const TokenType type, const char* message) {
  LITR_PROFILE_FUNCTION();

  if (m_current.type == type) {
    advance();
    return;
  }

  error_at_current(message);
}

bool Parser::match(const TokenType type) const {
  LITR_PROFILE_FUNCTION();

  return m_previous.type == type;
}

bool Parser::match(const std::initializer_list<TokenType> types) const {
  LITR_PROFILE_FUNCTION();

  return std::any_of(types.begin(), types.end(), [&](TokenType type) {
    return match(type);
  });
}

bool Parser::peak(const TokenType type) const {
  LITR_PROFILE_FUNCTION();

  return m_current.type == type;
}

void Parser::emit_byte(const std::byte byte) {
  LITR_PROFILE_FUNCTION();

  m_instruction->write(byte);
}

void Parser::emit_byte(const Instruction::Code code) {
  LITR_PROFILE_FUNCTION();

  m_instruction->write(code);
}

void Parser::emit_bytes(const Instruction::Code code, const std::byte byte) {
  LITR_PROFILE_FUNCTION();

  emit_byte(code);
  emit_byte(byte);
}

void Parser::emit_constant(const Instruction::Value& value) {
  LITR_PROFILE_FUNCTION();

  emit_bytes(Instruction::Code::CONSTANT, make_constant(value));
}

void Parser::emit_definition(const Instruction::Value& value) {
  LITR_PROFILE_FUNCTION();

  emit_bytes(Instruction::Code::DEFINE, make_constant(value));
}

void Parser::emit_scope(const Instruction::Value& value) {
  LITR_PROFILE_FUNCTION();

  m_scope.push_back(value);
  emit_bytes(Instruction::Code::BEGIN_SCOPE, make_constant(value));
}

void Parser::emit_execution() {
  LITR_PROFILE_FUNCTION();

  const std::string scope_path{get_scope_path()};
  emit_bytes(Instruction::Code::EXECUTE, make_constant(scope_path));
}

void Parser::emit_clear() {
  LITR_PROFILE_FUNCTION();

  emit_byte(Instruction::Code::CLEAR);
  m_scope.pop_back();
}

std::byte Parser::make_constant(const Instruction::Value& value) {
  LITR_PROFILE_FUNCTION();

  return m_instruction->write_constant(value);
}

// NOLINTNEXTLINE(misc-no-recursion)
void Parser::arguments() {
  LITR_PROFILE_FUNCTION();

  advance();

  if (match(TokenType::EQUAL)) {
    error("You are missing a parameter in front of the assignment.");
    return;
  }

  if (match({TokenType::STRING, TokenType::NUMBER, TokenType::ERROR})) {
    error("This is not allowed here.");
    return;
  }

  if (match(TokenType::COMMAND)) {
    commands();
    arguments();
  }

  if (match({TokenType::SHORT_PARAMETER, TokenType::LONG_PARAMETER})) {
    parameters();
    arguments();
  }

  if (m_previous.type == TokenType::COMMA) {
    comma();
    arguments();
  }
}

void Parser::commands() {
  LITR_PROFILE_FUNCTION();

  emit_scope(Scanner::get_token_value(m_previous));
}

void Parser::parameters() {
  LITR_PROFILE_FUNCTION();

  emit_definition(Utils::trim_left(Scanner::get_token_value(m_previous), '-'));

  if (peak(TokenType::EQUAL)) {
    advance();
    consume(TokenType::STRING, "Value assignment missing.");
    emit_constant(Utils::trim(Scanner::get_token_value(m_previous), '"'));
  }
}

void Parser::comma() {
  LITR_PROFILE_FUNCTION();

  if (m_scope.empty()) {
    error("Unexpected comma.");
    return;
  }

  if (peak(TokenType::COMMA)) {
    error_at_current("Duplicated comma.");
    return;
  }

  emit_execution();
  emit_clear();
}

void Parser::end_of_string() {
  LITR_PROFILE_FUNCTION();

  if (!m_scope.empty()) {
    emit_execution();
  }

  consume(TokenType::EOS, "Expected end.");
}

void Parser::error_at_current(const std::string& message) {
  LITR_PROFILE_FUNCTION();

  error_at(&m_current, message);
}

void Parser::error(const std::string& message) {
  LITR_PROFILE_FUNCTION();

  error_at(&m_previous, message);
}

void Parser::error_at(Token* token, const std::string& message) {
  LITR_PROFILE_FUNCTION();

  if (m_panic_mode) {
    return;
  }
  m_panic_mode = true;

  std::string out_message{"Cannot parse"};

  if (token->type == TokenType::EOS) {
    out_message.append(" at end");
  } else if (token->type == TokenType::ERROR) {
    // Nothing, yet.
  } else {
    out_message.append(fmt::format(" at `{}`", Scanner::get_token_value(token)));
  }

  out_message.append(fmt::format(": {}", message));

  Error::Handler::push(
      Error::CLIParserError(out_message, 1, token->column, Utils::trim(m_source, ' ')));

  m_has_error = true;
}

std::string Parser::get_scope_path() const {
  LITR_PROFILE_FUNCTION();

  std::string value{};

  for (size_t i{0}; i < m_scope.size(); ++i) {
    value.append(m_scope[i]);
    if (i < m_scope.size() - 1) {
      value.append(".");
    }
  }

  return value;
}

}  // namespace Litr::CLI
