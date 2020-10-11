#pragma once

#include <string>
#include <vector>

namespace Litr {

struct Parameters {
  enum class ParameterType { String = 0, Array = 1 };

  std::string Name;
  std::string Description;
  char Shortcut{};
  ParameterType Type{ParameterType::String};
  std::vector<std::string> TypeArguments{};
  std::string Default{};
};

}  // namespace Litr
