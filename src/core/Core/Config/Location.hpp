#pragma once

#include <string>
#include <utility>

namespace Litr::Config {

struct Location {
  uint32_t Line{};
  uint32_t Column{};
  std::string LineStr{};

  Location() = default;
  Location(uint32_t line, uint32_t column, std::string lineStr)
      : Line(line), Column(column), LineStr(std::move(lineStr)) {
  }
} __attribute__((aligned(32)));

}  // namespace Litr::Config
