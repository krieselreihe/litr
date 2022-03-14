#include <fmt/format.h>

#include "Toml.hpp"

std::pair<toml::table, toml::value> CreateTOMLMock(const std::string& name, const std::string& toml) {
  std::string rFile{fmt::format(R"([{}]
{})", name, toml)};

  std::istringstream isFile(rFile, std::ios_base::binary | std::ios_base::in);
  const auto file = toml::parse(isFile, "std::string");

  std::istringstream isData(toml, std::ios_base::binary | std::ios_base::in);
  const auto data = toml::parse(isData, "std::string");

  return {file.as_table(), data};
}
