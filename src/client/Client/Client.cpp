/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Client/Application.hpp"
#include "Core.hpp"

int main(int argc, char* argv[]) {
  LITR_PROFILE_BEGIN_SESSION_WITH_FILE("Litr", "litr-profile.json");

  litr::Application app{};
  litr::ExitStatus status{app.run(argc, argv)};

  LITR_PROFILE_END_SESSION();

  return static_cast<int>(status);
}
