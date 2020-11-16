#pragma once

#include <fmt/format.h>

#include <string>
#include <utility>
#include <vector>

#include "Core/Base.hpp"

namespace Litr {

struct Command {
  enum class Output { UNCHANGED = 0, SILENT = 1 };

  std::vector<std::string> Script{};
  std::vector<std::string> Directory{};

  const std::string Name;
  std::string Description{};
  std::string Example{};
  std::vector<Ref<Command>> ChildCommands{};

  Output Output{Output::UNCHANGED};

  explicit Command(std::string name) : Name(std::move(name)) {
  }
};

}  // namespace Litr

// Enable easy formatting with fmt
template <>
struct fmt::formatter<Litr::Command> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Litr::Command& c, FormatContext& ctx) {
    std::string childView{};
    if (!c.ChildCommands.empty()) {
      for (const auto& child : c.ChildCommands) {
        childView.append(fmt::format("\n    - Sub{}", *child));
      }
    }

    if (!c.Description.empty()) {
      return format_to(ctx.out(), "Script: {} - {}{}", c.Name, c.Description, childView);
    }

    return format_to(ctx.out(), "Script: {}{}", c.Name, childView);
  }
};