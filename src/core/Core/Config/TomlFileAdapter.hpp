/*
 * Copyright (c) 2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <tsl/ordered_map.h>

#include <toml.hpp>

#include "Core/Config/FileAdapter.hpp"

namespace Litr::Config {

using BasicTomlValue = toml::basic_value<toml::discard_comments, tsl::ordered_map>;

class TomlFileAdapter : public FileAdapter<BasicTomlValue> {
 public:
  using Exception = toml::exception;

  [[nodiscard]] Value parse(const Path& file_path) const override;

  [[nodiscard]] const Value& find(const Value& value, const std::string& key) const override;
};

}  // namespace Litr::Config
