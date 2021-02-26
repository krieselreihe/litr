#include "Scanner.hpp"

#include "Core/Debug/Instrumentor.hpp"

namespace Litr {

Scanner::Scanner(const char* source) : m_Start(source), m_Current(source) {
}

void Scanner::SkipWhitespace() {
  LITR_PROFILE_FUNCTION();

  for (;;) {
    char c = Peek();
    switch (c) {
      case ' ':
      case '\r':
      case '\t': {
        Advance();
        break;
      }
      default: {
        return;
      }
    }
  }
}

char Scanner::Peek() {
  LITR_PROFILE_FUNCTION();

  return *m_Current;
}

char Scanner::PeekNext() const {
  LITR_PROFILE_FUNCTION();

  if (IsAtEnd()) {
    return '\0';
  }

  return m_Current[1];
}

char Scanner::Advance() {
  LITR_PROFILE_FUNCTION();

  m_Current++;
  m_Column++;
  return m_Current[-1];
}

bool Scanner::Match(char expected) {
  LITR_PROFILE_FUNCTION();

  if (IsAtEnd()) {
    return false;
  }

  if (*m_Current != expected) {
    return false;
  }

  m_Current++;
  m_Column++;
  return true;
}

Token Scanner::ScanToken() {
  LITR_PROFILE_FUNCTION();

  SkipWhitespace();

  m_Start = m_Current;

  if (IsAtEnd()) {
    return MakeToken(TokenType::EOS);
  }

  char c = Advance();

  if (IsDigit(c)) return Number();
  if (IsAlpha(c)) return Command();

  switch (c) {
    case ',': return MakeToken(TokenType::COMMA);
    case '=': return MakeToken(TokenType::EQUAL);
    case '-': return Match('-') ? LongParameter() : ShortParameter();
    case '"': return String();
    default: return ErrorToken("Unexpected character.");
  }
}

std::string Scanner::GetTokenValue(const Token& token) {
  LITR_PROFILE_FUNCTION();

  return std::string(token.Start, token.Length);
}

std::string Scanner::GetTokenValue(Token* token) {
  LITR_PROFILE_FUNCTION();

  return std::string(token->Start, token->Length);
}

Token Scanner::MakeToken(TokenType type) const {
  LITR_PROFILE_FUNCTION();

  Token token{type, m_Start, static_cast<size_t>(m_Current - m_Start), m_Column};
  return token;
}

Token Scanner::ErrorToken(const char* message) const {
  LITR_PROFILE_FUNCTION();

  Token token{TokenType::ERROR, message, strlen(message), m_Column};
  return token;
}

bool Scanner::IsDigit(char c) {
  return c >= '0' && c <= '9';
}

bool Scanner::IsAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::IsShortAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Scanner::IsAtEnd() const {
  return *m_Current == '\0';
}

Token Scanner::String() {
  LITR_PROFILE_FUNCTION();

  while (Peek() != '"' && !IsAtEnd()) {
    Advance();
  }

  if (IsAtEnd()) {
    return ErrorToken("Unterminated string.");
  }

  // The closing quote.
  Advance();
  return MakeToken(TokenType::STRING);
}

Token Scanner::Number() {
  LITR_PROFILE_FUNCTION();

  while (IsDigit(Peek())) {
    Advance();
  }

  // Look for a fractional part.
  if (Peek() == '.' && IsDigit(PeekNext())) {
    // Consume the ".".
    Advance();

    while (IsDigit(Peek())) {
      Advance();
    }
  }

  return MakeToken(TokenType::NUMBER);
}

Token Scanner::Command() {
  LITR_PROFILE_FUNCTION();

  while (IsAlpha(Peek()) || IsDigit(Peek())) {
    Advance();
  }

  return MakeToken(TokenType::COMMAND);
}

Token Scanner::LongParameter() {
  LITR_PROFILE_FUNCTION();

  bool hasError{false};

  if (IsDigit(Peek()) || Peek() == '_') {
    Advance();
    hasError = true;
  }

  while (IsAlpha(Peek()) || IsDigit(Peek())) {
    Advance();
  }

  if (hasError) {
    return ErrorToken("A parameter cannot start with a number or _ (allowed characters are A-Za-z).");
  }

  return MakeToken(TokenType::LONG_PARAMETER);
}

Token Scanner::ShortParameter() {
  LITR_PROFILE_FUNCTION();

  if (!IsShortAlpha(Advance())) {
    return ErrorToken("A short parameter can only be A-Za-z as name.");
  }

  int length{1};
  while (IsAlpha(Peek())) {
    Advance();
    length++;
  }

  if (length > 1) {
    return ErrorToken("A short parameter can only contain one character (A-Za-z).");
  }

  return MakeToken(TokenType::SHORT_PARAMETER);
}

}  // namespace Litr
