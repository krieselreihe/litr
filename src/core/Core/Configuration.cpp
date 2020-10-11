#include "Configuration.hpp"

#include <fstream> //required for parse_file()
#include <iostream>

#include "Core/Log.hpp"

namespace Litr {

Configuration::Configuration(const Path& filePath) {
  toml::parse_result result{toml::parse_file(filePath.ToString())};

  if (!result) {
    // @todo: Errors needs to be parsed and wrapped to be maximum helpful to the user.
    // https://github.com/krieselreihe/litr/issues/11
    LITR_ERROR("Parsing failed in {} with {}", filePath, result.error().description());
    return;
  }

  m_RawConfig = std::move(result).table();

  std::cout << m_RawConfig << "\n";
}

}  // namespace Litr
