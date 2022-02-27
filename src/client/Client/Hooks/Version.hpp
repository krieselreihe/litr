/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include "Core.hpp"

namespace litr::hook {

class Version {
 public:
  static void print(const Ref<cli::Instruction>& instruction);
};

}  // namespace litr::hook
