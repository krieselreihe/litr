/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include <cstdlib>

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Environment.hpp"

namespace litr {

Path Environment::get_home_directory() {
  LITR_PROFILE_FUNCTION();

  // std::getenv is not thread safe, but this will not be a problem here.
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  return Path(std::getenv("HOME"));
}

}  // namespace litr
