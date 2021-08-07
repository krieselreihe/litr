#pragma once

#include "Core.hpp"

namespace Litr::Hook {

class Version {
 public:
  static void Print(const Ref<CLI::Instruction>& instruction);
};

}  // namespace Litr::Hook
