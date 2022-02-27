/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

namespace litr::script {

enum class TokenType {
  // Single-character tokens.
  LEFT_PAREN,
  RIGHT_PAREN,
  COMMA,

  // Literals.
  UNTOUCHED,
  IDENTIFIER,
  STRING,

  // Keywords.
  /* AND, */ OR,

  START_SEQ,
  END_SEQ,
  ERROR,
  EOS  // End of script
};

struct Token {
  TokenType type;
  const char* start{};
  size_t length{};
  uint32_t line{};
  uint32_t column{};
};

}  // namespace litr::script
