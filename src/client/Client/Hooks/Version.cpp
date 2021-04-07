#include "Version.hpp"

#include <fmt/printf.h>

#include "Core.hpp"

namespace Litr {

void Version::Print([[maybe_unused]] const Ref<CLI::Instruction>& instruction) {
  fmt::print("{}.{}.{}\n", LITR_VERSION_MAJOR, LITR_VERSION_MINOR, LITR_VERSION_PATCH);
}

}  // namespace Litr
