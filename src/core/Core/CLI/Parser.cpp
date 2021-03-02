#include "Parser.hpp"

#include "Core/CLI/Scanner.hpp"
#include "Core/Utils.hpp"

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Debug/Disassembler.hpp"

namespace Litr::CLI {

Parser::Parser(const Ref<ErrorHandler>& errorHandler, const Ref<Instruction>& instruction, const std::string& source)
    : m_Scanner(source.c_str()), m_ErrorHandler(errorHandler), m_Instruction(instruction) {
  LITR_PROFILE_FUNCTION();

  Advance();
  Arguments();
  EndOfString();

  LITR_DISASSEMBLE(m_Instruction, "Test instructions");
}

void Parser::Advance() {
  LITR_PROFILE_FUNCTION();

  m_Previous = m_Current;

  while (true) {
    m_Current = m_Scanner.ScanToken();

    if (m_Current.Type != TokenType::ERROR) {
      break;
    }

    ErrorAtCurrent(m_Current.Start);
  }
}

void Parser::Consume(TokenType type, const char* message) {
  LITR_PROFILE_FUNCTION();

  if (m_Current.Type == type) {
    Advance();
    return;
  }

  ErrorAtCurrent(message);
}

bool Parser::Match(TokenType type) const {
  LITR_PROFILE_FUNCTION();

  return m_Previous.Type == type;
}

bool Parser::Match(std::initializer_list<TokenType> types) const {
  LITR_PROFILE_FUNCTION();

  return std::any_of(types.begin(), types.end(), [&](TokenType type) {
    return Match(type);
  });
}

bool Parser::Peak(TokenType type) const {
  LITR_PROFILE_FUNCTION();

  return m_Current.Type == type;
}

void Parser::EmitByte(std::byte byte) {
  LITR_PROFILE_FUNCTION();

  m_Instruction->Write(byte);
}

void Parser::EmitByte(Instruction::Code code) {
  LITR_PROFILE_FUNCTION();

  m_Instruction->Write(code);
}

void Parser::EmitBytes(Instruction::Code code, std::byte byte) {
  LITR_PROFILE_FUNCTION();

  EmitByte(code);
  EmitByte(byte);
}

void Parser::EmitConstant(const Instruction::Value& value) {
  LITR_PROFILE_FUNCTION();

  EmitBytes(Instruction::Code::CONSTANT, MakeConstant(value));
}

void Parser::EmitDefinition(const Instruction::Value& value) {
  LITR_PROFILE_FUNCTION();

  EmitBytes(Instruction::Code::DEFINE, MakeConstant(value));
}

void Parser::EmitScope(const Instruction::Value& value) {
  LITR_PROFILE_FUNCTION();

  m_Scope.push_back(value);
  EmitBytes(Instruction::Code::BEGIN_SCOPE, MakeConstant(value));
}

void Parser::EmitExecution() {
  LITR_PROFILE_FUNCTION();

  const std::string scopePath{GetScopePath()};
  EmitBytes(Instruction::Code::EXECUTE, MakeConstant(scopePath));
}

void Parser::EmitClear() {
  LITR_PROFILE_FUNCTION();

  EmitByte(Instruction::Code::CLEAR);
  m_Scope.pop_back();
}

std::byte Parser::MakeConstant(const Instruction::Value& value) {
  LITR_PROFILE_FUNCTION();

  return m_Instruction->WriteConstant(value);
}

// Ignore recursive call action
// NOLINTNEXTLINE
void Parser::Arguments() {
  LITR_PROFILE_FUNCTION();

  Advance();

  if (Match(TokenType::EQUAL)) {
    Error("You are missing a parameter in front of the assignment.");
    return;
  }

  if (Match({TokenType::STRING, TokenType::NUMBER, TokenType::ERROR})) {
    Error("This is not allowed here.");
    return;
  }

  if (Match(TokenType::COMMAND)) {
    Commands();
    Arguments();
  }

  if (Match({TokenType::SHORT_PARAMETER, TokenType::LONG_PARAMETER})) {
    Parameters();
    Arguments();
  }

  if (m_Previous.Type == TokenType::COMMA) {
    Comma();
    Arguments();
  }
}

void Parser::Commands() {
  LITR_PROFILE_FUNCTION();

  EmitScope(Scanner::GetTokenValue(m_Previous));
}

void Parser::Parameters() {
  LITR_PROFILE_FUNCTION();

  EmitDefinition(Utils::TrimLeft(Scanner::GetTokenValue(m_Previous), '-'));

  if (Peak(TokenType::EQUAL)) {
    Advance();
    Consume(TokenType::STRING, "Value assignment missing.");
    EmitConstant(Utils::Trim(Scanner::GetTokenValue(m_Previous), '"'));
  }
}

void Parser::Comma() {
  LITR_PROFILE_FUNCTION();

  if (m_Scope.empty()) {
    Error("Unexpected comma.");
    return;
  }

  if (Peak(TokenType::COMMA)) {
    ErrorAtCurrent("Duplicated comma.");
    return;
  }

  EmitExecution();
  EmitClear();
}

void Parser::EndOfString() {
  LITR_PROFILE_FUNCTION();

  if (!m_Scope.empty()) EmitExecution();
  Consume(TokenType::EOS, "Expected end.");
}

void Parser::ErrorAtCurrent(const char* message) {
  LITR_PROFILE_FUNCTION();

  ErrorAt(&m_Current, message);
}

void Parser::Error(const char* message) {
  LITR_PROFILE_FUNCTION();

  ErrorAt(&m_Previous, message);
}

void Parser::ErrorAt(Token* token, const char* message) {
  LITR_PROFILE_FUNCTION();

  if (m_PanicMode) return;
  m_PanicMode = true;

  std::string outMessage{"Cannot parse"};

  if (token->Type == TokenType::EOS) {
    outMessage.append(" at end");
  } else if (token->Type == TokenType::ERROR) {
    // Nothing, yet.
  } else {
    outMessage.append(fmt::format(" at '{}'", Scanner::GetTokenValue(token)));
  }

  outMessage.append(fmt::format(": {}\n", message));

  m_ErrorHandler->Push({
      ErrorType::PARSER_ERROR,
      outMessage,
      1,
      0, // token->Column,
      // @todo: Shows currently only shortened line string.
      std::string(token->Start)
  });

  m_HasError = true;
}

std::string Parser::GetScopePath() const {
  LITR_PROFILE_FUNCTION();

  std::string value{};

  for (size_t i{0}; i < m_Scope.size(); ++i) {
    value.append(m_Scope[i]);
    if (i < m_Scope.size() - 1) {
      value.append(".");
    }
  }

  return value;
}

}  // namespace Litr::CLI
