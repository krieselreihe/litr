/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Scanner.hpp"

#include <cstring>

#include "Core/Debug/Instrumentor.hpp"

namespace litr::script {

Scanner::Scanner(const char* source) : m_start(source), m_current(source) {
}

void Scanner::skip_whitespace() {
  LITR_PROFILE_FUNCTION();

  for (;;) {
    char c{peek()};
    switch (c) {
      case '\n':
        m_line++;
        m_column = 0;
        advance();
        break;
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

char Scanner::peek() const {
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

  switch (m_modes.top()) {
    case Mode::UNTOUCHED:  return scan_untouched_token();
    case Mode::EXPRESSION: return scan_expression_token();
  }

  return {};
}

Token Scanner::scan_untouched_token() {
  LITR_PROFILE_FUNCTION();

  m_start = m_current;

  if (is_at_end()) {
    return make_token(TokenType::EOS);
  }

  char c{advance()};

  if (c == '%' && match('{')) {
    return start_sequence();
  }

  return untouched();
}

Token Scanner::scan_expression_token() {
  LITR_PROFILE_FUNCTION();

  skip_whitespace();

  m_start = m_current;

  if (is_at_end()) {
    return make_token(TokenType::EOS);
  }

  char c{advance()};

  if (is_alpha(c)) return identifier();

  switch (c) {
    case ',': return make_token(TokenType::COMMA);
    case '(': return make_token(TokenType::LEFT_PAREN);
    case ')': return make_token(TokenType::RIGHT_PAREN);
    case '}': return end_sequence();
    case '\'': return string();
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

  Token token{type, m_start, static_cast<size_t>(m_current - m_start), m_line, m_column};
  return token;
}

Token Scanner::error_token(const char* message) const {
  LITR_PROFILE_FUNCTION();

  Token token{TokenType::ERROR, message, strlen(message), m_line, m_column};
  return token;
}

bool Scanner::is_digit(char c) {
  LITR_PROFILE_FUNCTION();

  return c >= '0' && c <= '9';
}

bool Scanner::is_alpha(char c) {
  LITR_PROFILE_FUNCTION();

  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::is_at_end() const {
  LITR_PROFILE_FUNCTION();

  return *m_current == '\0';
}

Token Scanner::start_sequence() {
  LITR_PROFILE_FUNCTION();

  m_modes.push(Mode::EXPRESSION);
  return make_token(TokenType::START_SEQ);
}

Token Scanner::end_sequence() {
  LITR_PROFILE_FUNCTION();

  m_modes.pop();
  return make_token(TokenType::END_SEQ);
}

Token Scanner::untouched() {
  LITR_PROFILE_FUNCTION();

  // @todo: Refactor this condition for the check `m_Current[-1] != '\\'` of the escape sequence.
  while (
      !(peek() == '%' && peek_next() == '{' && m_current[-1] != '\\')
      && !is_at_end()) {
    advance();
  }

  return make_token(TokenType::UNTOUCHED);
}

Token Scanner::string() {
  LITR_PROFILE_FUNCTION();

  while (peek() != '\'' && !is_at_end()) {
    advance();
  }

  if (is_at_end()) {
    return error_token("Unterminated string.");
  }

  // The closing quote.
  advance();
  return make_token(TokenType::STRING);
}

Token Scanner::identifier() {
  LITR_PROFILE_FUNCTION();

  while (is_alpha(peek()) || is_digit(peek())) { advance();
  }

  return make_token(identifier_type());
}

TokenType Scanner::identifier_type() const {
  LITR_PROFILE_FUNCTION();

  switch (m_start[0]) {
    // case 'a': return CheckKeyword(1, 2, "nd", TokenType::AND);
    case 'o': return check_keyword(1, 1, "r", TokenType::OR);
  }

  return TokenType::IDENTIFIER;
}

TokenType Scanner::check_keyword(size_t start, size_t length, const char* rest, TokenType type) const {
  LITR_PROFILE_FUNCTION();

  if (m_current - m_start == static_cast<int16_t>(start + length)
      && std::memcmp(m_start + start, rest, length) == 0) {
    return type;
  }

  return TokenType::IDENTIFIER;
}

}  // namespace litr::script
