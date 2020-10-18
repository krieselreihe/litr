#pragma once

#include <fmt/format.h>

#include <string>
#include <utility>
#include <vector>

namespace Litr {

struct Command {
  enum class Output { UNCHANGED = 0, SILENT = 1 };

  std::string Name;
  std::vector<std::string> Script{};
  std::string Description{};
  std::string Example{};
  std::vector<std::string> Directory{};
  Output Output{Output::UNCHANGED};
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
    if (!c.Description.empty()) {
      return format_to(ctx.out(), "Script: {} - {}", c.Name, c.Description);
    }

    return format_to(ctx.out(), "Script: {}", c.Name);
  }
};
