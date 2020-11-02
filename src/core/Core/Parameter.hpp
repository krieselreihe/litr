#pragma once

#include <fmt/format.h>

#include <string>
#include <vector>

namespace Litr {

struct Parameter {
  enum class ParameterType { String = 0, Array = 1 };

  std::vector<std::string> TypeArguments{};

  std::string Name;
  std::string Description{};
  std::string Shortcut{};
  std::string Default{};

  ParameterType Type{ParameterType::String};

  explicit Parameter(std::string name) : Name(std::move(name)) {}
};

}  // namespace Litr

// Enable easy formatting with fmt
template <>
struct fmt::formatter<Litr::Parameter> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Litr::Parameter& p, FormatContext& ctx) {
    if (!p.Description.empty()) {
      return format_to(ctx.out(), "Param: {} - {}", p.Name, p.Description);
    }

    return format_to(ctx.out(), "Param: {}", p.Name);
  }
};
