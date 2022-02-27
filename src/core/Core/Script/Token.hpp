/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

namespace litr::Script {

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
  TokenType Type;
  const char* Start{};
  size_t Length{};
  uint32_t Line{};
  uint32_t Column{};
};

}  // namespace litr::Script
