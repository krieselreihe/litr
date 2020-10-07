#include <cstdlib>
#include <fmt/format.h>

#include "Core/Environment.hpp"

namespace Litr {

Path Environment::GetHomeDirectory() {
  std::string userName{std::getenv("USER")};
  std::string loginName{std::getenv("LOGNAME")};
  Path path{fmt::format("/Users/{}", userName.empty() ? userName : loginName)};

  return path;
}

}  // namespace Litr
