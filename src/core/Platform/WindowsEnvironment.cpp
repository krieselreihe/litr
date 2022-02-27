/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include <cstdlib>

#include "Core/Environment.hpp"
#include "Core/Debug/Instrumentor.hpp"

namespace litr {

Path Environment::GetHomeDirectory() {
  LITR_PROFILE_FUNCTION();

  return Path(std::getenv("HOMEPATH"));
}

}  // namespace litr
