/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Compiler.hpp"

#include <fmt/format.h>

#include <utility>
#include <algorithm>

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Error/Handler.hpp"
#include "Core/Utils.hpp"

namespace litr::Script {

Compiler::Compiler(const std::string& source, Config::Location location, Variables variables)
    : m_Scanner(source.c_str()), m_Location(std::move(location)), m_Variables(std::move(variables)) {
  LITR_PROFILE_FUNCTION();

  Advance();
  Source();
  EndOfScript();
}

std::string Compiler::GetScript() const {
  return m_Script;
}

std::vector<std::string> Compiler::GetUsedVariables() const {
  return m_UsedVariables;
}

void Compiler::Advance() {
  LITR_PROFILE_FUNCTION();

  m_Previous = m_Current;

  while (true) {
    m_Current = m_Scanner.ScanToken();

    if (m_Current.Type != TokenType::ERROR) {
      break;
    }

    ErrorAtCurrent(m_Current.Start);
  }
}

void Compiler::Consume(TokenType type, const char *message) {
  LITR_PROFILE_FUNCTION();

  if (m_Current.Type == type) {
    Advance();
    return;
  }

  ErrorAtCurrent(message);
}

bool Compiler::Match(TokenType type) const {
  LITR_PROFILE_FUNCTION();

  return m_Previous.Type == type;
}

bool Compiler::Peak(TokenType type) const {
  LITR_PROFILE_FUNCTION();

  return m_Current.Type == type;
}

// Ignore recursive call action
// NOLINTNEXTLINE
void Compiler::Source() {
  LITR_PROFILE_FUNCTION();

  Advance();

  if (Match(TokenType::UNTOUCHED)) {
    Untouched();
    Source();
    return;
  }

  if (Match(TokenType::START_SEQ)) {
    Script();
    return;
  }
}

void Compiler::Untouched() {
  LITR_PROFILE_FUNCTION();

  m_Script.append(Scanner::GetTokenValue(m_Previous));
}

// Ignore recursive call action
// NOLINTNEXTLINE
void Compiler::Script() {
  LITR_PROFILE_FUNCTION();

  Advance();

  switch (m_Previous.Type) {
    case TokenType::STRING:
      String();
      break;
    case TokenType::IDENTIFIER:
      Identifier();
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
      Error("Unexpected character.");
      break;
  }

  EndOfSequence();
  Source();
}

// Ignore recursive call action
// NOLINTNEXTLINE
void Compiler::Identifier() {
  LITR_PROFILE_FUNCTION();

  const std::string name{Scanner::GetTokenValue(m_Previous)};
  auto variable{m_Variables.find(name)};

  if (variable == m_Variables.end()) {
    Error("Undefined parameter.");
    return;
  }

  CollectUsedVariable(variable->second);

  switch (variable->second.Type) {
    case CLI::Variable::Type::STRING:
      String(variable->second);
      break;
    case CLI::Variable::Type::BOOLEAN:
      Statement(variable->second);
      break;
  }
}

// Ignore recursive call action
// NOLINTNEXTLINE
void Compiler::Statement(const CLI::Variable& variable) {
  LITR_PROFILE_FUNCTION();

  Advance();

  if (Peak(TokenType::OR)) {
    OrStatement(variable);
  } else {
    IfStatement(variable);
  }
}

// Ignore recursive call action
// NOLINTNEXTLINE
void Compiler::OrStatement(const CLI::Variable& variable) {
  LITR_PROFILE_FUNCTION();

  if (std::get<bool>(variable.Value)) {
    Expression();
    Consume(TokenType::OR, R"(Expected `or` after expression.)");
    // Skip "false" clause
    Advance();
  } else {
    // Skip over true branch
    while (!Peak(TokenType::OR) && !Peak(TokenType::EOS)) {
      Advance();
    }

    Consume(TokenType::OR, R"(Expected `or` after expression.)");
    Advance();
    Expression();
  }
}

// Ignore recursive call action
// NOLINTNEXTLINE
void Compiler::IfStatement(const CLI::Variable& variable) {
  LITR_PROFILE_FUNCTION();

  if (std::get<bool>(variable.Value)) {
    Expression();
  }
}

// Ignore recursive call action
// NOLINTNEXTLINE
void Compiler::Expression() {
  LITR_PROFILE_FUNCTION();

  if (Match(TokenType::STRING)) {
    String();
    return;
  }

  if (Match(TokenType::IDENTIFIER)) {
    Identifier();
    return;
  }
}

void Compiler::String() {
  LITR_PROFILE_FUNCTION();

  m_Script.append(Utils::Trim(Scanner::GetTokenValue(m_Previous), '\''));
}

void Compiler::String(const CLI::Variable& variable) {
  LITR_PROFILE_FUNCTION();

  m_Script.append(std::get<std::string>(variable.Value));
}

void Compiler::EndOfSequence() {
  LITR_PROFILE_FUNCTION();

  Consume(TokenType::END_SEQ, "Expected `}`.");
}

void Compiler::EndOfScript() {
  LITR_PROFILE_FUNCTION();

  Consume(TokenType::EOS, "Expected end.");
}

void Compiler::CollectUsedVariable(const CLI::Variable& variable) {
  // If name is not already collected:
  if (std::find(m_UsedVariables.begin(), m_UsedVariables.end(), variable.Name) == m_UsedVariables.end()) {
    m_UsedVariables.push_back(variable.Name);
  }
}

void Compiler::Error(const std::string& message) {
  LITR_PROFILE_FUNCTION();

  ErrorAt(&m_Previous, message);
}

void Compiler::ErrorAtCurrent(const std::string& message) {
  LITR_PROFILE_FUNCTION();

  ErrorAt(&m_Current, message);
}

void Compiler::ErrorAt(Token *token, const std::string& message) {
  LITR_PROFILE_FUNCTION();

  if (m_PanicMode) return;
  m_PanicMode = true;

  std::string outMessage{"Cannot parse"};

  if (token->Type == TokenType::EOS) {
    outMessage.append(" at end");
  } else if (token->Type == TokenType::ERROR) {
    // Nothing, yet.
  } else {
    outMessage.append(fmt::format(" at `{}`", Scanner::GetTokenValue(token)));
  }

  outMessage.append(fmt::format(": {}", message));

  Error::Handler::Push(Error::ScriptParserError(
      outMessage,
      m_Location.Line,
      m_Location.Column + token->Column + 1,
      m_Location.LineStr
  ));
}

}  // namespace litr::Script
