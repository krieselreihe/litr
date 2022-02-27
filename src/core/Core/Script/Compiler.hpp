/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Core/CLI/Variable.hpp"
#include "Core/Config/Location.hpp"
#include "Core/Script/Token.hpp"
#include "Core/Script/Scanner.hpp"

namespace litr::script {

class Compiler {
 public:
  using Variables = std::unordered_map<std::string, cli::Variable>;

  Compiler(const std::string& source, config::Location location, Variables variables);

  [[nodiscard]] std::string get_script() const;
  [[nodiscard]] std::vector<std::string> get_used_variables() const;

 private:
  void advance();
  void consume(TokenType type, const char* message);

  [[nodiscard]] bool match(TokenType type) const;
  [[nodiscard]] bool peak(TokenType type) const;

  void source_token();
  void untouched();
  void script();
  void identifier();

  void statement(const cli::Variable& variable);
  void or_statement(const cli::Variable& variable);
  void if_statement(const cli::Variable& variable);

  void expression();
  void string();
  void string(const cli::Variable& variable);
  void end_of_sequence();
  void end_of_script();

  void collect_used_variable(const cli::Variable& variable);

  void error(const std::string& message);
  void error_at_current(const std::string& message);
  void error_at(Token* token, const std::string& message);

 private:
  Scanner m_scanner;
  const config::Location m_location;
  Variables m_variables;

  Token m_current{};
  Token m_previous{};

  bool m_panic_mode{false};
  std::string m_script{};
  std::vector<std::string> m_used_variables{};
};

}  // namespace litr::script
