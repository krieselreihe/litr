/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Client/Application.hpp"
#include "Core.hpp"

// @todo: This seems to only be a problem when building a profiling build.
//  I'm not sure why but for now I deactivate it.
// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char* argv[]) {
  LITR_PROFILE_BEGIN_SESSION_WITH_FILE("Litr", "litr-profile.json");

  // Incremented argv plus 1 to skip the program name.
  const std::vector<std::string> arguments{argv + 1, argv + argc};

  Litr::Application app{};
  const Litr::ExitStatus status{app.run(arguments)};

  LITR_PROFILE_END_SESSION();

  return static_cast<int>(status);
}
