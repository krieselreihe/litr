/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>
#include <stack>

#include "Core/Script/Token.hpp"

namespace Litr::Script {

class Scanner {
 public:
  explicit Scanner(const char* source);

  [[nodiscard]] Token ScanToken();
  [[nodiscard]] Token ScanUntouchedToken();
  [[nodiscard]] Token ScanExpressionToken();

  [[nodiscard]] static std::string GetTokenValue(const Token& token);
  [[nodiscard]] static std::string GetTokenValue(Token* token);

 private:
  enum class Mode { UNTOUCHED, EXPRESSION };

  void SkipWhitespace();
  char Advance();
  [[nodiscard]] char Peek() const;
  [[nodiscard]] char PeekNext() const;
  [[nodiscard]] bool Match(char expected);

  [[nodiscard]] Token MakeToken(TokenType type) const;
  [[nodiscard]] Token ErrorToken(const char* message) const;

  [[nodiscard]] static bool IsAlpha(char c);
  [[nodiscard]] static bool IsDigit(char c);
  [[nodiscard]] bool IsAtEnd() const;

  [[nodiscard]] Token StartSequence();
  [[nodiscard]] Token EndSequence();
  [[nodiscard]] Token Untouched();
  [[nodiscard]] Token String();
  [[nodiscard]] Token Identifier();

  [[nodiscard]] TokenType IdentifierType() const;
  [[nodiscard]] TokenType CheckKeyword(size_t start, size_t length, const char* rest, TokenType type) const;

 private:
  const char* m_Start;
  const char* m_Current;

  uint32_t m_Line{1};
  uint32_t m_Column{1};

  std::stack<Mode> m_Modes{{Mode::UNTOUCHED}};
};

}  // namespace Litr::Script
