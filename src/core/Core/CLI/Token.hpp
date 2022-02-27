/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

namespace litr::CLI {

enum class TokenType {
  COMMA,
  EQUAL,
  COMMAND,
  SHORT_PARAMETER,
  LONG_PARAMETER,
  STRING,
  NUMBER,
  ERROR,
  EOS  // End of string
};

struct Token {
  TokenType Type;
  const char* Start;
  size_t Length;
  uint32_t Column;
};

}  // namespace litr::CLI
