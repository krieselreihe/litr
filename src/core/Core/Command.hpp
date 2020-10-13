#pragma once

#include <string>
#include <utility>
#include <vector>

namespace Litr {

struct Command {
  enum class Output { UNCHANGED = 0, SILENT = 1 };

  std::string Name;
  std::vector<std::string> Script;
  std::string Description{};
  std::string Example{};
  std::vector<std::string> Directory{};
  Output Output{Output::UNCHANGED};
};

}  // namespace Litr
