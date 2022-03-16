/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Scanner.hpp"

#include "Core/Debug/Instrumentor.hpp"

namespace litr::cli {

Scanner::Scanner(const char* source) : m_start(source), m_current(source) {
}

void Scanner::skip_whitespace() {
  LITR_PROFILE_FUNCTION();

  for (;;) {
    char c{peek()};
    switch (c) {
      case ' ':
      case '\r':
      case '\t': {
        advance();
        break;
      }
      default: {
        return;
      }
    }
  }
}

char Scanner::peek() {
  LITR_PROFILE_FUNCTION();

  return *m_current;
}

char Scanner::peek_next() const {
  LITR_PROFILE_FUNCTION();

  if (is_at_end()) {
    return '\0';
  }

  return m_current[1];
}

char Scanner::advance() {
  LITR_PROFILE_FUNCTION();

  m_current++;
  m_column++;
  return m_current[-1];
}

bool Scanner::match(char expected) {
  LITR_PROFILE_FUNCTION();

  if (is_at_end()) {
    return false;
  }

  if (*m_current != expected) {
    return false;
  }

  m_current++;
  m_column++;
  return true;
}

Token Scanner::scan_token() {
  LITR_PROFILE_FUNCTION();

  skip_whitespace();

  m_start = m_current;

  if (is_at_end()) {
    return make_token(TokenType::EOS);
  }

  char c{advance()};

  if (is_digit(c)) return number();
  if (is_alpha(c)) return command();

  switch (c) {
    case ',': return make_token(TokenType::COMMA);
    case '=': return make_token(TokenType::EQUAL);
    case '-': return match('-') ? long_parameter() : short_parameter();
    case '"': return string();
    default: return error_token("Unexpected character.");
  }
}

std::string Scanner::get_token_value(const Token& token) {
  LITR_PROFILE_FUNCTION();

  return {token.start, token.length};
}

std::string Scanner::get_token_value(Token* token) {
  LITR_PROFILE_FUNCTION();

  return {token->start, token->length};
}

Token Scanner::make_token(TokenType type) const {
  LITR_PROFILE_FUNCTION();

  Token token{type, m_start, static_cast<size_t>(m_current - m_start), m_column};
  return token;
}

Token Scanner::error_token(const char* message) const {
  LITR_PROFILE_FUNCTION();

  Token token{TokenType::ERROR, message, strlen(message), m_column};
  return token;
}

bool Scanner::is_digit(char c) {
  return c >= '0' && c <= '9';
}

bool Scanner::is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::is_short_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Scanner::is_at_end() const {
  return *m_current == '\0';
}

Token Scanner::string() {
  LITR_PROFILE_FUNCTION();

  while (peek() != '"' && !is_at_end()) {
    advance();
  }

  if (is_at_end()) {
    return error_token("Unterminated string.");
  }

  // The closing quote.
  advance();
  return make_token(TokenType::STRING);
}

Token Scanner::number() {
  LITR_PROFILE_FUNCTION();

  while (is_digit(peek())) {
    advance();
  }

  // Look for a fractional part.
  if (peek() == '.' && is_digit(peek_next())) {
    // Consume the ".".
    advance();

    while (is_digit(peek())) {
      advance();
    }
  }

  return make_token(TokenType::NUMBER);
}

Token Scanner::command() {
  LITR_PROFILE_FUNCTION();

  while (is_alpha(peek()) || is_digit(peek())) {
    advance();
  }

  return make_token(TokenType::COMMAND);
}

Token Scanner::long_parameter() {
  LITR_PROFILE_FUNCTION();

  bool hasError{false};

  if (is_digit(peek()) || peek() == '_' || !is_alpha(peek())) {
    advance();
    hasError = true;
  }

  while (is_alpha(peek()) || is_digit(peek())) {
    advance();
  }

  if (hasError) {
    return error_token("A parameter can only start with the characters A-Za-z.");
  }

  return make_token(TokenType::LONG_PARAMETER);
}

Token Scanner::short_parameter() {
  LITR_PROFILE_FUNCTION();

  if (!is_short_alpha(advance())) {
    return error_token("A short parameter can only be A-Za-z as name.");
  }

  int length{1};
  while (is_alpha(peek())) {
    advance();
    length++;
  }

  if (length > 1) {
    return error_token("A short parameter can only contain one character (A-Za-z).");
  }

  return make_token(TokenType::SHORT_PARAMETER);
}

}  // namespace litr::cli
