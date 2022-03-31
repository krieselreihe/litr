/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include "Core/FileSystem.hpp"

namespace Litr {

class Environment {
 public:
  [[nodiscard]] static Path get_home_directory();
};

}  // namespace Litr
