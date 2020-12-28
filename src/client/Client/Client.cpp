#include "Core.hpp"
#include "Client/Application.hpp"

int main(int argc, char* argv[]) {
  LITR_PROFILE_BEGIN_SESSION_WITH_FILE("Litr", "litr-profile.json");

  Litr::Application app{};
  int exitStatus{app.Run(argc, argv)};

  LITR_PROFILE_END_SESSION();

  return exitStatus;
}
