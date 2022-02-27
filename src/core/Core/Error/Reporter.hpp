/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <vector>

#include "Core/Error/BaseError.hpp"
#include "Core/FileSystem.hpp"

namespace litr::error {

class Reporter {
 public:
  explicit Reporter(const Path& path);

  void print_errors(const std::vector<BaseError>& errors);
  void print_error(const BaseError& error);

 private:
  static uint32_t count_digits(uint32_t number);

  const Path m_file_path;
  bool m_multiple_errors{false};
};

}  // namespace litr::error
