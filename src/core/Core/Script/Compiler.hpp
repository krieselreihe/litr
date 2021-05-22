#pragma once

#include <string>
#include <unordered_map>

#include "Core/CLI/Variable.hpp"
#include "Core/Config/Location.hpp"
#include "Core/Script/Token.hpp"
#include "Core/Script/Scanner.hpp"

namespace Litr::Script {

class Compiler {
 public:
  using Variables = std::unordered_map<std::string, CLI::Variable>;

  Compiler(const std::string& source, Config::Location location, Variables variables);
  [[nodiscard]] std::string GetScript() const;
  [[nodiscard]] std::vector<std::string> GetUsedVariables() const;

 private:
  void Advance();
  void Consume(TokenType type, const char* message);

  [[nodiscard]] bool Match(TokenType type) const;
  [[nodiscard]] bool Peak(TokenType type) const;

  void Source();
  void Untouched();
  void Script();
  void Identifier();

  void Statement(const CLI::Variable& variable);
  void OrStatement(const CLI::Variable& variable);
  void IfStatement(const CLI::Variable& variable);

  void Expression();
  void String();
  void String(const CLI::Variable& variable);
  void EndOfSequence();
  void EndOfScript();

  void Error(const std::string& message);
  void ErrorAtCurrent(const std::string& message);
  void ErrorAt(Token* token, const std::string& message);

 private:
  Scanner m_Scanner;
  const Config::Location m_Location;
  Variables m_Variables;

  Token m_Current{};
  Token m_Previous{};

  bool m_PanicMode{false};
  std::string m_Script{};
  std::vector<std::string> m_UsedVariables{};
};

}  // namespace Litr::Script
