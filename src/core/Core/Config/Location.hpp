/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>
#include <utility>

namespace litr::config {

struct Location {
  uint32_t line{};
  uint32_t column{};
  std::string line_str{};

  Location() = default;
  Location(uint32_t line, uint32_t column, std::string lineStr)
      : line(line), column(column), line_str(std::move(lineStr)) {
  }
};

}  // namespace litr::config
