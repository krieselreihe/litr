#pragma once

#include <fmt/format.h>

#include <string>
#include <utility>
#include <vector>

#include "Core/Base.hpp"
#include "Core/Config/Location.hpp"

namespace Litr::Config {

struct Command {
  enum class Output { UNCHANGED = 0, SILENT = 1 };

  std::vector<std::string> Script{};
  std::vector<std::string> Directory{};

  const std::string Name;
  std::string Description{};
  std::string Example{};
  std::vector<Ref<Command>> ChildCommands{};

  Output Output{Output::UNCHANGED};
  std::vector<Location> Locations{};

  explicit Command(std::string name) : Name(std::move(name)) {
  }
} __attribute__((aligned(128)));

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
    std::string childView{};
    if (!c.ChildCommands.empty()) {
      for (auto&& child : c.ChildCommands) {
        childView.append(fmt::format("\n    - Sub{}", *child));
      }
    }

    if (!c.Description.empty()) {
      return format_to(ctx.out(), "Script: {} - {}{}", c.Name, c.Description, childView);
    }

    return format_to(ctx.out(), "Script: {}{}", c.Name, childView);
  }
};
#endif
