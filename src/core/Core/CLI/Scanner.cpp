/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Scanner.hpp"

#include "Core/Debug/Instrumentor.hpp"

namespace Litr::CLI {

Scanner::Scanner(const char* source) : m_start(source), m_current(source) {}

void Scanner::skip_whitespace() {
  LITR_PROFILE_FUNCTION();

  for (;;) {
    char character{peek()};
    switch (character) {
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

  ++m_current;
  ++m_column;
  return m_current[-1];
}

bool Scanner::match(const char expected) {
  LITR_PROFILE_FUNCTION();

  if (is_at_end()) {
    return false;
  }

  if (*m_current != expected) {
    return false;
  }

  ++m_current;
  ++m_column;
  return true;
}

Token Scanner::scan_token() {
  LITR_PROFILE_FUNCTION();

  skip_whitespace();

  m_start = m_current;

  if (is_at_end()) {
    return make_token(TokenType::EOS);
  }

  char character{advance()};

  if (is_digit(character)) {
    return number();
  }

  if (is_alpha(character)) {
    return command();
  }

  switch (character) {
    case ',':
      return make_token(TokenType::COMMA);
    case '=':
      return make_token(TokenType::EQUAL);
    case '-':
      return match('-') ? long_parameter() : short_parameter();
    case '"':
      return string();
    default:
      return error_token("Unexpected character.");
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

Token Scanner::make_token(const TokenType type) const {
  LITR_PROFILE_FUNCTION();

  Token token{type, m_start, static_cast<size_t>(m_current - m_start), m_column};
  return token;
}

Token Scanner::error_token(const char* message) const {
  LITR_PROFILE_FUNCTION();

  Token token{TokenType::ERROR, message, strlen(message), m_column};
  return token;
}

bool Scanner::is_digit(const char character) {
  return character >= '0' && character <= '9';
}

bool Scanner::is_alpha(const char character) {
  return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') ||
         character == '_';
}

bool Scanner::is_short_alpha(const char character) {
  return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z');
}

bool Scanner::is_hyphen(char character) {
  return character == '-';
}

bool Scanner::is_underscore(char character) {
  return character == '_';
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

  // The closing quotation mark.
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

  while (is_alpha(peek()) || is_digit(peek()) || is_hyphen(peek())) {
    advance();
  }

  return make_token(TokenType::COMMAND);
}

Token Scanner::long_parameter() {
  LITR_PROFILE_FUNCTION();

  bool has_error{false};

  if (is_digit(peek()) || is_underscore(peek()) || !is_alpha(peek())) {
    advance();
    has_error = true;
  }

  while (is_alpha(peek()) || is_digit(peek()) || is_hyphen(peek())) {
    advance();
  }

  if (has_error) {
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
    ++length;
  }

  if (length > 1) {
    return error_token("A short parameter can only contain one character (A-Za-z).");
  }

  return make_token(TokenType::SHORT_PARAMETER);
}

}  // namespace Litr::CLI
