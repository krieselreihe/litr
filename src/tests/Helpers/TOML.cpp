/*
 * Copyright (c) 2020 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "TOML.hpp"

#include <fmt/format.h>
#include <tsl/ordered_map.h>

#include <toml.hpp>

TomlMock create_toml_mock(const std::string& name, const std::string& toml) {
  std::string r_context{fmt::format(R"([{}]
{})",
      name,
      toml)};

  litr::config::TomlFileAdapter file{};

  std::istringstream is_context(r_context, std::ios_base::binary | std::ios_base::in);
  const auto context =
      toml::parse<toml::discard_comments, tsl::ordered_map>(is_context, "std::string");

  std::istringstream is_data(toml, std::ios_base::binary | std::ios_base::in);
  const auto data = toml::parse<toml::discard_comments, tsl::ordered_map>(is_data, "std::string");

  return {context, data};
}
