/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include <fmt/format.h>

#include <cstdlib>

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Environment.hpp"

namespace Litr {

static Path user_path(const std::string& user_name) {
  Path path{fmt::format("/Users/{}", user_name)};
  return path;
}

Path Environment::get_home_directory() {
  LITR_PROFILE_FUNCTION();

  // std::getenv is not thread safe, but this will not be a problem here.
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  const std::string user_name{std::getenv("USER")};
  if (!user_name.empty()) {
    return user_path(user_name);
  }

  // std::getenv is not thread safe, but this will not be a problem here.
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  const std::string login_name{std::getenv("LOGNAME")};
  if (!login_name.empty()) {
    return user_path(login_name);
  }

  return {};
}

}  // namespace Litr
