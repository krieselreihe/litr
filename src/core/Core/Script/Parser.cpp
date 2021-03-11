#include "Parser.hpp"

#include <utility>

namespace Litr::Script {

Parser::Parser(std::string script, std::vector<CLI::Variable> variables)
    : m_Script(std::move(script)), m_Variables(std::move(variables)) {
}

std::string Parser::GetScript() const {
  return m_Script;
}

}  // namespace Litr::Script
