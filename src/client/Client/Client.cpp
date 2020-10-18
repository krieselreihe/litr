#include <fmt/printf.h>

#include "Core.hpp"

int main() {
  LITR_PROFILE_BEGIN_SESSION_WITH_FILE("Litr", "litr-profile.json");
  Litr::Log::Init();

  fmt::print("Hello, Litr!\n");

  {
    LITR_PROFILE_SCOPE("ClientRunner");

    Litr::Path cwd{Litr::FileSystem::GetCurrentWorkingDirectory()};
    Litr::ConfigFileResolver configPath{cwd};

    switch (configPath.GetStatus()) {
      case Litr::ConfigFileResolver::Status::NOT_FOUND: {
        fmt::print("No configuration file found!\n");
        return EXIT_FAILURE;
      }
      case Litr::ConfigFileResolver::Status::DUPLICATE: {
        fmt::print(
            "You defined both, litr.toml and .litr.toml in {0}."
            "This is probably an error and you only want one of them.\n",
            configPath.GetFileDirectory());
        return EXIT_FAILURE;
      }
      case Litr::ConfigFileResolver::Status::FOUND: {
        fmt::print("Configuration file found under: {0}\n", configPath.GetFilePath());
      }
    }

    // Quick test ...
    auto config{Litr::CreateScope<Litr::Configuration>(configPath.GetFilePath())};
    if (config->HasErrors()) {
      Litr::ErrorReporter::PrintErrors(config->GetErrors());
    }
  }

  LITR_PROFILE_END_SESSION();

  return EXIT_SUCCESS;
}
