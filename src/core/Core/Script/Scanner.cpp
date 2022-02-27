/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Scanner.hpp"

#include <cstring>

#include "Core/Debug/Instrumentor.hpp"

namespace Litr::Script {

Scanner::Scanner(const char* source) : m_Start(source), m_Current(source) {
}

void Scanner::SkipWhitespace() {
  LITR_PROFILE_FUNCTION();

  for (;;) {
    char c{Peek()};
    switch (c) {
      case '\n':
        m_Line++;
        m_Column = 0;
        Advance();
        break;
      case ' ':
      case '\r':
      case '\t': {
        Advance();
        break;
      }
      default: {
        return;
      }
    }
  }
}

char Scanner::Peek() const {
  LITR_PROFILE_FUNCTION();

  return *m_Current;
}

char Scanner::PeekNext() const {
  LITR_PROFILE_FUNCTION();

  if (IsAtEnd()) {
    return '\0';
  }

  return m_Current[1];
}

char Scanner::Advance() {
  LITR_PROFILE_FUNCTION();

  m_Current++;
  m_Column++;
  return m_Current[-1];
}

bool Scanner::Match(char expected) {
  LITR_PROFILE_FUNCTION();

  if (IsAtEnd()) {
    return false;
  }

  if (*m_Current != expected) {
    return false;
  }

  m_Current++;
  m_Column++;
  return true;
}

Token Scanner::ScanToken() {
  LITR_PROFILE_FUNCTION();

  switch (m_Modes.top()) {
    case Mode::UNTOUCHED:  return ScanUntouchedToken();
    case Mode::EXPRESSION: return ScanExpressionToken();
  }

  return {};
}

Token Scanner::ScanUntouchedToken() {
  LITR_PROFILE_FUNCTION();

  m_Start = m_Current;

  if (IsAtEnd()) {
    return MakeToken(TokenType::EOS);
  }

  char c{Advance()};

  if (c == '%' && Match('{')) {
    return StartSequence();
  }

  return Untouched();
}

Token Scanner::ScanExpressionToken() {
  LITR_PROFILE_FUNCTION();

  SkipWhitespace();

  m_Start = m_Current;

  if (IsAtEnd()) {
    return MakeToken(TokenType::EOS);
  }

  char c{Advance()};

  if (IsAlpha(c)) return Identifier();

  switch (c) {
    case ',': return MakeToken(TokenType::COMMA);
    case '(': return MakeToken(TokenType::LEFT_PAREN);
    case ')': return MakeToken(TokenType::RIGHT_PAREN);
    case '}': return EndSequence();
    case '\'': return String();
    default: return ErrorToken("Unexpected character.");
  }
}

std::string Scanner::GetTokenValue(const Token& token) {
  LITR_PROFILE_FUNCTION();

  return {token.Start, token.Length};
}

std::string Scanner::GetTokenValue(Token* token) {
  LITR_PROFILE_FUNCTION();

  return {token->Start, token->Length};
}

Token Scanner::MakeToken(TokenType type) const {
  LITR_PROFILE_FUNCTION();

  Token token{type, m_Start, static_cast<size_t>(m_Current - m_Start), m_Line, m_Column};
  return token;
}

Token Scanner::ErrorToken(const char* message) const {
  LITR_PROFILE_FUNCTION();

  Token token{TokenType::ERROR, message, strlen(message), m_Line, m_Column};
  return token;
}

bool Scanner::IsDigit(char c) {
  LITR_PROFILE_FUNCTION();

  return c >= '0' && c <= '9';
}

bool Scanner::IsAlpha(char c) {
  LITR_PROFILE_FUNCTION();

  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::IsAtEnd() const {
  LITR_PROFILE_FUNCTION();

  return *m_Current == '\0';
}

Token Scanner::StartSequence() {
  LITR_PROFILE_FUNCTION();

  m_Modes.push(Mode::EXPRESSION);
  return MakeToken(TokenType::START_SEQ);
}

Token Scanner::EndSequence() {
  LITR_PROFILE_FUNCTION();

  m_Modes.pop();
  return MakeToken(TokenType::END_SEQ);
}

Token Scanner::Untouched() {
  LITR_PROFILE_FUNCTION();

  // @todo: Refactor this condition for the check `m_Current[-1] != '\\'` of the escape sequence.
  while (
      !(Peek() == '%' && PeekNext() == '{' && m_Current[-1] != '\\')
      && !IsAtEnd()) {
    Advance();
  }

  return MakeToken(TokenType::UNTOUCHED);
}

Token Scanner::String() {
  LITR_PROFILE_FUNCTION();

  while (Peek() != '\'' && !IsAtEnd()) {
    Advance();
  }

  if (IsAtEnd()) {
    return ErrorToken("Unterminated string.");
  }

  // The closing quote.
  Advance();
  return MakeToken(TokenType::STRING);
}

Token Scanner::Identifier() {
  LITR_PROFILE_FUNCTION();

  while (IsAlpha(Peek()) || IsDigit(Peek())) {
    Advance();
  }

  return MakeToken(IdentifierType());
}

TokenType Scanner::IdentifierType() const {
  LITR_PROFILE_FUNCTION();

  switch (m_Start[0]) {
    // case 'a': return CheckKeyword(1, 2, "nd", TokenType::AND);
    case 'o': return CheckKeyword(1, 1, "r", TokenType::OR);
  }

  return TokenType::IDENTIFIER;
}

TokenType Scanner::CheckKeyword(size_t start, size_t length, const char* rest, TokenType type) const {
  LITR_PROFILE_FUNCTION();

  if (m_Current - m_Start == static_cast<int16_t>(start + length)
      && std::memcmp(m_Start + start, rest, length) == 0) {
    return type;
  }

  return TokenType::IDENTIFIER;
}

}  // namespace Litr::Script
