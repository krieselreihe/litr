/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include "Core.hpp"

namespace litr::Hook {

class Version {
 public:
  static void Print(const Ref<CLI::Instruction>& instruction);
};

}  // namespace litr::Hook
