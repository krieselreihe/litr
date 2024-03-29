/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

namespace Litr::CLI {

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
  TokenType type;
  const char* start;
  size_t length;
  uint32_t column;
};

}  // namespace Litr::CLI
