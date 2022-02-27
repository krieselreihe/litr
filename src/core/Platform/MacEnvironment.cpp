/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include <fmt/format.h>

#include <cstdlib>

#include "Core/Environment.hpp"
#include "Core/Debug/Instrumentor.hpp"

namespace litr {

static Path UserPath(const std::string& user_name) {
  const Path path{fmt::format("/Users/{}", user_name)};
  return path;
}

Path Environment::get_home_directory() {
  LITR_PROFILE_FUNCTION();

  const std::string user_name{std::getenv("USER")};
  if (!user_name.empty()) {
    return UserPath(user_name);
  }

  const std::string login_name{std::getenv("LOGNAME")};
  if (!login_name.empty()) {
    return UserPath(login_name);
  }

  return {};
}

}  // namespace litr
