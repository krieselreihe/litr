#pragma once

#include <string>

#include "Core/CLI/Interpreter.hpp"
#include "Core/Config/Location.hpp"

namespace Litr::Script {

class Parser {
 public:
  explicit Parser(std::string script, Config::Location location, std::vector<CLI::Variable> variables);
  [[nodiscard]] std::string GetScript() const;

 private:
  const std::string m_Script;
  const Config::Location m_Location;
  const std::vector<CLI::Variable> m_Variables;
};

}  // namespace Litr::Script
