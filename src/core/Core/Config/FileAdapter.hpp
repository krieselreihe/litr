/*
 * Copyright (c) 2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <tsl/ordered_map.h>

#include <string>

#include "Core/FileSystem.hpp"

namespace litr::config {

template <typename T>
class FileAdapter {
 public:
  using Value = T;
  using Table = tsl::ordered_map<std::string, Value>;

  virtual ~FileAdapter() = default;

  virtual Value parse(const Path& file_path) const = 0;

  virtual const Value& find_value(const Value&, const std::string& key) const = 0;
  virtual const Table& find_table(const Value&, const std::string& key) const = 0;
};

}  // namespace litr::config
