/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Version.hpp"

#include <fmt/format.h>

#include "Core.hpp"

namespace Litr::Hook {

std::string Version::read() {
  return fmt::format("{}.{}.{}\n", LITR_VERSION_MAJOR, LITR_VERSION_MINOR, LITR_VERSION_PATCH);
}

}  // namespace Litr::Hook
