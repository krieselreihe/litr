/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <fmt/format.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Core/Config/Location.hpp"

namespace Litr::Config {

struct Command {
  enum class Output { UNCHANGED = 0, SILENT = 1 };

  std::vector<std::string> script{};
  std::vector<std::string> directory{};

  const std::string name;
  std::string description{};
  std::string example{};
  std::vector<std::shared_ptr<Command>> child_commands{};

  Output output{Output::UNCHANGED};
  std::vector<Location> Locations{};

  explicit Command(std::string name) : name(std::move(name)) {}
};

}  // namespace Litr::Config

#ifdef DEBUG
// Enable easy formatting with fmt
template <>
struct fmt::formatter<Litr::Config::Command> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Litr::Config::Command& c, FormatContext& ctx) {
    std::string child_view{};
    if (!c.child_commands.empty()) {
      for (auto&& child : c.child_commands) {
        child_view.append(fmt::format("\n    - Child{}", *child));
      }
    }

    if (!c.description.empty()) {
      return format_to(ctx.out(), "Script: {} - {}{}", c.name, c.description, child_view);
    }

    return format_to(ctx.out(), "Script: {}{}", c.name, child_view);
  }
};
#endif
