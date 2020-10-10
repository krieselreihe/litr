#pragma once

#include "Core/FileSystem.hpp"

namespace Litr {

class Environment {
 public:
  [[nodiscard]] static Path GetHomeDirectory();
};

}  // namespace Litr
