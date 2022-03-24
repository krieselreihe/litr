/*
 * Copyright (c) 2020 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "TOML.hpp"

#include <fmt/format.h>

std::pair<toml::table, toml::value> create_toml_mock(
    const std::string& name, const std::string& toml) {
  std::string r_file{fmt::format(R"([{}]
{})",
      name,
      toml)};

  std::istringstream is_file(r_file, std::ios_base::binary | std::ios_base::in);
  const auto file = toml::parse(is_file, "std::string");

  std::istringstream is_data(toml, std::ios_base::binary | std::ios_base::in);
  const auto data = toml::parse(is_data, "std::string");

  return {file.as_table(), data};
}
