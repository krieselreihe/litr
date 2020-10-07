#include <cstdlib>
#include <fmt/format.h>

#include "Core/Environment.hpp"
#include "Core/Debug/Instrumentor.hpp"

namespace Litr {

Path Environment::GetHomeDirectory() {
  LITR_PROFILE_FUNCTION();

  std::string userName{std::getenv("USER")};
  std::string loginName{std::getenv("LOGNAME")};
  Path path{fmt::format("/Users/{}", userName.empty() ? userName : loginName)};

  return path;
}

}  // namespace Litr
