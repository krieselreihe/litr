#include "Parser.hpp"

#include <utility>

namespace Litr::Script {

Parser::Parser(std::string script, std::vector<CLI::Variable> variables)
    : m_Script(std::move(script)), m_Variables(std::move(variables)) {
  // @todo:
  // - On error, show place in config line where the error is
  // - Properly parse inside %{}, but ignore outside.
  // - Have possibility to escape %{} sequence via \%{}
  // - Define a set of functions to manipulate strings
  // - Simple boolean expressions via %{variable ''} / %{variable '' or ''}
}

std::string Parser::GetScript() const {
  return m_Script;
}

}  // namespace Litr::Script
