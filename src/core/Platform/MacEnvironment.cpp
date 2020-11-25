#include <cstdlib>
#include <fmt/format.h>

#include "Core/Environment.hpp"
#include "Core/Debug/Instrumentor.hpp"

namespace Litr {

static Path UserPath(const std::string& userName) {
  const Path path{fmt::format("/Users/{}", userName)};
  return path;
}

Path Environment::GetHomeDirectory() {
  LITR_PROFILE_FUNCTION();

  const std::string userName{std::getenv("USER")};
  if (!userName.empty()) {
    return UserPath(userName);
  }

  const std::string loginName{std::getenv("LOGNAME")};
  if (!loginName.empty()) {
    return UserPath(loginName);
  }

  return Path();
}

}  // namespace Litr
