/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Compiler.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <utility>

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Error/Handler.hpp"
#include "Core/Utils.hpp"

namespace litr::script {

Compiler::Compiler(const std::string& source, config::Location location, Variables variables)
    : m_scanner(source.c_str()),
      m_location(std::move(location)),
      m_variables(std::move(variables)) {
  LITR_PROFILE_FUNCTION();

  advance();
  source_token();
  end_of_script();
}

std::string Compiler::get_script() const {
  return m_script;
}

std::vector<std::string> Compiler::get_used_variables() const {
  return m_used_variables;
}

void Compiler::advance() {
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

void Compiler::consume(const TokenType type, const char* message) {
  LITR_PROFILE_FUNCTION();

  if (m_current.type == type) {
    advance();
    return;
  }

  error_at_current(message);
}

bool Compiler::match(const TokenType type) const {
  LITR_PROFILE_FUNCTION();

  return m_previous.type == type;
}

bool Compiler::peak(const TokenType type) const {
  LITR_PROFILE_FUNCTION();

  return m_current.type == type;
}

// NOLINTNEXTLINE(misc-no-recursion)
void Compiler::source_token() {
  LITR_PROFILE_FUNCTION();

  advance();

  if (match(TokenType::UNTOUCHED)) {
    untouched();
    source_token();
    return;
  }

  if (match(TokenType::START_SEQ)) {
    script();
    return;
  }
}

void Compiler::untouched() {
  LITR_PROFILE_FUNCTION();

  m_script.append(Scanner::get_token_value(m_previous));
}

// NOLINTNEXTLINE(misc-no-recursion)
void Compiler::script() {
  LITR_PROFILE_FUNCTION();

  advance();

  switch (m_previous.type) {
    case TokenType::STRING:
      string();
      break;
    case TokenType::IDENTIFIER:
      identifier();
      break;
    case TokenType::LEFT_PAREN:
    case TokenType::RIGHT_PAREN:
    case TokenType::COMMA:
    case TokenType::UNTOUCHED:
    case TokenType::OR:
    case TokenType::START_SEQ:
    case TokenType::END_SEQ:
    case TokenType::ERROR:
    case TokenType::EOS:
      error("Unexpected character.");
      break;
  }

  end_of_sequence();
  source_token();
}

// NOLINTNEXTLINE(misc-no-recursion)
void Compiler::identifier() {
  LITR_PROFILE_FUNCTION();

  const std::string name{Scanner::get_token_value(m_previous)};
  auto variable{m_variables.find(name)};

  if (variable == m_variables.end()) {
    error("Undefined parameter.");
    return;
  }

  collect_used_variable(variable->second);

  switch (variable->second.type) {
    case cli::Variable::Type::STRING:
      string(variable->second);
      break;
    case cli::Variable::Type::BOOLEAN:
      statement(variable->second);
      break;
  }
}

// NOLINTNEXTLINE(misc-no-recursion)
void Compiler::statement(const cli::Variable& variable) {
  LITR_PROFILE_FUNCTION();

  advance();

  if (peak(TokenType::OR)) {
    or_statement(variable);
  } else {
    if_statement(variable);
  }
}

// NOLINTNEXTLINE(misc-no-recursion)
void Compiler::or_statement(const cli::Variable& variable) {
  LITR_PROFILE_FUNCTION();

  if (std::get<bool>(variable.value)) {
    expression();
    consume(TokenType::OR, R"(Expected `or` after expression.)");
    // Skip "false" clause
    advance();
  } else {
    // Skip over true branch
    while (!peak(TokenType::OR) && !peak(TokenType::EOS)) {
      advance();
    }

    consume(TokenType::OR, R"(Expected `or` after expression.)");
    advance();
    expression();
  }
}

// NOLINTNEXTLINE(misc-no-recursion)
void Compiler::if_statement(const cli::Variable& variable) {
  LITR_PROFILE_FUNCTION();

  if (std::get<bool>(variable.value)) {
    expression();
  }
}

// NOLINTNEXTLINE(misc-no-recursion)
void Compiler::expression() {
  LITR_PROFILE_FUNCTION();

  if (match(TokenType::STRING)) {
    string();
    return;
  }

  if (match(TokenType::IDENTIFIER)) {
    identifier();
    return;
  }
}

void Compiler::string() {
  LITR_PROFILE_FUNCTION();

  m_script.append(utils::trim(Scanner::get_token_value(m_previous), '\''));
}

void Compiler::string(const cli::Variable& variable) {
  LITR_PROFILE_FUNCTION();

  m_script.append(std::get<std::string>(variable.value));
}

void Compiler::end_of_sequence() {
  LITR_PROFILE_FUNCTION();

  consume(TokenType::END_SEQ, "Expected `}`.");
}

void Compiler::end_of_script() {
  LITR_PROFILE_FUNCTION();

  consume(TokenType::EOS, "Expected end.");
}

void Compiler::collect_used_variable(const cli::Variable& variable) {
  // If name is not already collected:
  if (std::find(m_used_variables.begin(), m_used_variables.end(), variable.name) ==
      m_used_variables.end()) {
    m_used_variables.push_back(variable.name);
  }
}

void Compiler::error(const std::string& message) {
  LITR_PROFILE_FUNCTION();

  error_at(&m_previous, message);
}

void Compiler::error_at_current(const std::string& message) {
  LITR_PROFILE_FUNCTION();

  error_at(&m_current, message);
}

void Compiler::error_at(Token* token, const std::string& message) {
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

  error::Handler::push(error::ScriptParserError(
      out_message, m_location.line, m_location.column + token->column + 1, m_location.line_str));
}

}  // namespace litr::script
