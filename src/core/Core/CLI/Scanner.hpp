/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>

#include "Core/CLI/Token.hpp"

namespace litr::CLI {

class Scanner {
 public:
  explicit Scanner(const char* source);

  [[nodiscard]] Token ScanToken();
  [[nodiscard]] static std::string GetTokenValue(const Token& token);
  [[nodiscard]] static std::string GetTokenValue(Token* token);

 private:
  void SkipWhitespace();
  char Advance();
  char Peek();
  [[nodiscard]] char PeekNext() const;
  [[nodiscard]] bool Match(char expected);

  [[nodiscard]] Token MakeToken(TokenType type) const;
  [[nodiscard]] Token ErrorToken(const char* message) const;

  [[nodiscard]] static bool IsDigit(char c);
  [[nodiscard]] static bool IsAlpha(char c);
  [[nodiscard]] static bool IsShortAlpha(char c);
  [[nodiscard]] bool IsAtEnd() const;

  [[nodiscard]] Token String();
  [[nodiscard]] Token Number();
  [[nodiscard]] Token Command();
  [[nodiscard]] Token LongParameter();
  [[nodiscard]] Token ShortParameter();

 private:
  const char* m_Start;
  const char* m_Current;
  uint32_t m_Column{1};
};

}  // namespace litr::CLI
