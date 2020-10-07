#include <cstdlib>

#include "Core/Environment.hpp"

namespace Litr {

Path Environment::GetHomeDirectory() {
  return std::getenv("HOMEPATH");
}

}  // namespace Litr
