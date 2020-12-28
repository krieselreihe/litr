#include "Scanner.hpp"

namespace Litr {

Scanner::Scanner(const char* source) : m_Start(source), m_Current(source) {
}

void Scanner::SkipWhitespace() {
  for (;;) {
    char c = Peek();
    switch (c) {
      case ' ':
      case '\r':
      case '\t': {
        Advance();
        break;
      }
      case '\n': {
        m_Line++;
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
  return *m_Current;
}

char Scanner::PeekNext() const {
  if (IsAtEnd()) {
    return '\0';
  }

  return m_Current[1];
}

char Scanner::Advance() {
  m_Current++;
  return m_Current[-1];
}

bool Scanner::Match(char expected) {
  if (IsAtEnd()) {
    return false;
  }

  if (*m_Current != expected) {
    return false;
  }

  m_Current++;
  return true;
}

Scanner::Token Scanner::ScanToken() {
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

std::string Scanner::GetTokenValue(const Scanner::Token& token) {
  return std::string(token.Start, static_cast<unsigned long>(token.Length));
}

Scanner::Token Scanner::MakeToken(Scanner::TokenType type) const {
  Token token{type, m_Start, static_cast<int>(m_Current - m_Start), m_Line};
  return token;
}

Scanner::Token Scanner::ErrorToken(const char* message) const {
  Token token{TokenType::ERROR, message, static_cast<int>(strlen(message)), m_Line};
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

Scanner::Token Scanner::String() {
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

Scanner::Token Scanner::Number() {
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

Scanner::Token Scanner::Command() {
  while (IsAlpha(Peek()) || IsDigit(Peek())) {
    Advance();
  }

  return MakeToken(TokenType::COMMAND);
}

Scanner::Token Scanner::LongParameter() {
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

Scanner::Token Scanner::ShortParameter() {
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
