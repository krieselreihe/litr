/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core.hpp"

#include "Client/Application.hpp"

int main(int argc, char* argv[]) {
  LITR_PROFILE_BEGIN_SESSION_WITH_FILE("Litr", "litr-profile.json");

  litr::Application app{};
  litr::ExitStatus status{app.Run(argc, argv)};

  LITR_PROFILE_END_SESSION();

  return static_cast<int>(status);
}
