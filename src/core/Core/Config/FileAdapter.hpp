/*
 * Copyright (c) 2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>

#include "Core/FileSystem.hpp"

namespace Litr::Config {

template <typename T>
class FileAdapter {
 public:
  using Value = T;

  virtual ~FileAdapter() = default;

  virtual Value parse(const Path& file_path) const = 0;

  virtual const Value& find(const Value&, const std::string& key) const = 0;
};

}  // namespace Litr::Config
