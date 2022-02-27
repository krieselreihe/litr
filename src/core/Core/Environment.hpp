/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include "Core/FileSystem.hpp"

namespace Litr {

class Environment {
 public:
  [[nodiscard]] static Path GetHomeDirectory();
};

}  // namespace Litr
