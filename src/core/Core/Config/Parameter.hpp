/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <fmt/format.h>

#include <string>
#include <utility>
#include <vector>

namespace Litr::Config {

struct Parameter {
  enum class Type { STRING = 0, BOOLEAN = 1, ARRAY = 2 };

  std::vector<std::string> type_arguments{};

  const std::string name;
  std::string description{};
  std::string shortcut{};
  std::string default_value{};

  Type type{Type::STRING};

  explicit Parameter(std::string name) : name(std::move(name)) {}
};

}  // namespace Litr::Config

#ifdef DEBUG
// Enable easy formatting with fmt
template <>
struct fmt::formatter<Litr::Config::Parameter> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Litr::Config::Parameter& p, FormatContext& ctx) {
    if (!p.description.empty()) {
      return format_to(ctx.out(), "Param: {} - {}", p.name, p.description);
    }

    return format_to(ctx.out(), "Param: {}", p.name);
  }
};
#endif
