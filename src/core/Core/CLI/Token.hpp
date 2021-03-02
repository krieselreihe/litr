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
  TokenType Type;
  const char* Start;
  size_t Length;
  uint32_t Column;
} __attribute__((aligned(32))) __attribute__((packed));

}  // namespace Litr::CLI
