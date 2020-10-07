#include <cstdlib>

#include "Core/Environment.hpp"
#include "Core/Debug/Instrumentor.hpp"

namespace Litr {

Path Environment::GetHomeDirectory() {
  LITR_PROFILE_FUNCTION();

  return std::getenv("HOMEPATH");
}

}  // namespace Litr
