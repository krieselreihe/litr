#pragma once

#include <fmt/format.h>

#include <string>
#include <vector>

namespace Litr::Config {

struct Parameter {
  enum class ParameterType { String = 0, Array = 1 };

  std::vector<std::string> TypeArguments{};

  const std::string Name;
  std::string Description{};
  std::string Shortcut{};
  std::string Default{};

  ParameterType Type{ParameterType::String};

  explicit Parameter(std::string name) : Name(std::move(name)) {
  }
} __attribute__((aligned(128)));

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
    if (!p.Description.empty()) {
      return format_to(ctx.out(), "Param: {} - {}", p.Name, p.Description);
    }

    return format_to(ctx.out(), "Param: {}", p.Name);
  }
};
#endif
