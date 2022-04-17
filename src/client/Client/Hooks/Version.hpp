/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <memory>

#include "Core.hpp"

namespace Litr::Hook {

class Version {
 public:
  [[nodiscard]] static std::string read();
};

}  // namespace Litr::Hook
