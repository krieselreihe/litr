/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>

#include "Core/CLI/Token.hpp"

namespace Litr::CLI {

class Scanner {
 public:
  explicit Scanner(const char* source);

  [[nodiscard]] Token scan_token();
  [[nodiscard]] static std::string get_token_value(const Token& token);
  [[nodiscard]] static std::string get_token_value(Token* token);

 private:
  void skip_whitespace();
  char advance();
  char peek();
  [[nodiscard]] char peek_next() const;
  [[nodiscard]] bool match(char expected);

  [[nodiscard]] Token make_token(TokenType type) const;
  [[nodiscard]] Token error_token(const char* message) const;

  [[nodiscard]] static bool is_digit(char character);
  [[nodiscard]] static bool is_alpha(char character);
  [[nodiscard]] static bool is_short_alpha(char character);
  [[nodiscard]] static bool is_hyphen(char character);
  [[nodiscard]] static bool is_underscore(char character);
  [[nodiscard]] bool is_at_end() const;

  [[nodiscard]] Token string();
  [[nodiscard]] Token number();
  [[nodiscard]] Token command();
  [[nodiscard]] Token long_parameter();
  [[nodiscard]] Token short_parameter();

  const char* m_start;
  const char* m_current;
  uint32_t m_column{1};
};

}  // namespace Litr::CLI
