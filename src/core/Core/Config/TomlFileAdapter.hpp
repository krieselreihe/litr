/*
 * Copyright (c) 2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <tsl/ordered_map.h>

#include <toml.hpp>

#include "Core/Config/FileAdapter.hpp"

namespace litr::config {

using BasicTomlValue = toml::basic_value<toml::discard_comments, tsl::ordered_map>;

class TomlFileAdapter : public FileAdapter<BasicTomlValue> {
 public:
  using Exception = toml::exception;

  [[nodiscard]] Value parse(const Path& file_path) const override;

  [[nodiscard]] const Value& find_value(const Value& value, const std::string& key) const override;
  [[nodiscard]] const Table& find_table(const Value& value, const std::string& key) const override;
};

}  // namespace litr::config
