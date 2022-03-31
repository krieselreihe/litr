/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <memory>

#include "Core.hpp"

namespace Litr::Hook {

class Version {
 public:
  static void print(const std::shared_ptr<CLI::Instruction>& instruction);
};

}  // namespace Litr::Hook
