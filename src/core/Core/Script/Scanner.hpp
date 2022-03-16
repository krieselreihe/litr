/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <stack>
#include <string>

#include "Core/Script/Token.hpp"

namespace litr::script {

class Scanner {
 public:
  explicit Scanner(const char* source);

  [[nodiscard]] Token scan_token();
  [[nodiscard]] Token scan_untouched_token();
  [[nodiscard]] Token scan_expression_token();

  [[nodiscard]] static std::string get_token_value(const Token& token);
  [[nodiscard]] static std::string get_token_value(Token* token);

 private:
  enum class Mode { UNTOUCHED, EXPRESSION };

  void skip_whitespace();
  char advance();
  [[nodiscard]] char peek() const;
  [[nodiscard]] char peek_next() const;
  [[nodiscard]] bool match(char expected);

  [[nodiscard]] Token make_token(TokenType type) const;
  [[nodiscard]] Token error_token(const char* message) const;

  [[nodiscard]] static bool is_alpha(char c);
  [[nodiscard]] static bool is_digit(char c);
  [[nodiscard]] bool is_at_end() const;

  [[nodiscard]] Token start_sequence();
  [[nodiscard]] Token end_sequence();
  [[nodiscard]] Token untouched();
  [[nodiscard]] Token string();
  [[nodiscard]] Token identifier();

  [[nodiscard]] TokenType identifier_type() const;
  [[nodiscard]] TokenType check_keyword(
      size_t start, size_t length, const char* rest, TokenType type) const;

 private:
  const char* m_start;
  const char* m_current;

  uint32_t m_line{1};
  uint32_t m_column{1};

  std::stack<Mode> m_modes{{Mode::UNTOUCHED}};
};

}  // namespace litr::script
