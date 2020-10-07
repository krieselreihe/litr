#pragma once

#include <filesystem>

namespace Litr {

using Path = std::filesystem::path;

class Environment {
 public:
  [[nodiscard]] static std::filesystem::path GetHomeDirectory();
};

}  // namespace Litr
