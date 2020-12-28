#pragma once

#include <string>

namespace Litr {

class Scanner {
 public:
  enum class TokenType {
    COMMA, EQUAL,
    COMMAND, SHORT_PARAMETER, LONG_PARAMETER, STRING, NUMBER,
    ERROR,
    EOS  // End of string
  };

  struct Token {
    TokenType Type;
    const char* Start;
    int Length;
    int Line;
  } __attribute__((aligned(32))) __attribute__((packed));

  explicit Scanner(const char* source);

  [[nodiscard]] Token ScanToken();
  [[nodiscard]] static std::string GetTokenValue(const Token& token);

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
  int m_Line{1};
};

}  // namespace Litr
