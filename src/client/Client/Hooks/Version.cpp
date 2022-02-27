/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Version.hpp"

#include <fmt/printf.h>

#include "Core.hpp"

namespace litr::hook {

void Version::print([[maybe_unused]] const Ref<cli::Instruction>& instruction) {
  fmt::print("{}.{}.{}\n", LITR_VERSION_MAJOR, LITR_VERSION_MINOR, LITR_VERSION_PATCH);
}

}  // namespace litr::hook
