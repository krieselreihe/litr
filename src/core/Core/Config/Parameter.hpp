/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <fmt/format.h>

#include <string>
#include <vector>
#include <utility>

namespace litr::Config {

struct Parameter {
  enum class Type { STRING = 0, BOOLEAN = 1, ARRAY = 2 };

  std::vector<std::string> TypeArguments{};

  const std::string Name;
  std::string Description{};
  std::string Shortcut{};
  std::string Default{};

  Type Type{Type::STRING};

  explicit Parameter(std::string name) : Name(std::move(name)) {
  }
};

}  // namespace litr::Config

#ifdef DEBUG
// Enable easy formatting with fmt
template <>
struct fmt::formatter<litr::Config::Parameter> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const litr::Config::Parameter& p, FormatContext& ctx) {
    if (!p.Description.empty()) {
      return format_to(ctx.out(), "Param: {} - {}", p.Name, p.Description);
    }

    return format_to(ctx.out(), "Param: {}", p.Name);
  }
};
#endif
