/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>
#include <utility>

namespace litr::Config {

struct Location {
  uint32_t Line{};
  uint32_t Column{};
  std::string LineStr{};

  Location() = default;
  Location(uint32_t line, uint32_t column, std::string lineStr)
      : Line(line), Column(column), LineStr(std::move(lineStr)) {
  }
};

}  // namespace litr::Config
