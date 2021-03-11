#pragma once

#include <string>

#include "Core/CLI/Interpreter.hpp"

namespace Litr::Script {

class Parser {
 public:
  explicit Parser(std::string script, std::vector<CLI::Variable> variables);
  [[nodiscard]] std::string GetScript() const;

 private:
  const std::string m_Script;
  const std::vector<CLI::Variable> m_Variables;
};

}  // namespace Litr::Script
