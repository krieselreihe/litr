#include "Core.hpp"

int main() {
  LITR_PROFILE_BEGIN_SESSION_WITH_FILE("Litr", "litr-profile.json");
  Litr::Log::Init();

  LITR_INFO("Hello, Litr!");

  {
    LITR_PROFILE_SCOPE("ClientRunner");

    Litr::Path cwd{Litr::FileSystem::GetCurrentWorkingDirectory()};
    Litr::ConfigFile config{cwd};

    switch (config.GetStatus()) {
      case Litr::ConfigFile::Status::NOT_FOUND: {
        LITR_WARN("No configuration file found!");
        return EXIT_FAILURE;
      }
      case Litr::ConfigFile::Status::DUPLICATE: {
        LITR_WARN(
            "You defined both, litr.toml and .litr.toml in {0}."
            "This is probably an error and you only want one of them.",
            config.GetFileDirectory());
        return EXIT_FAILURE;
      }
      case Litr::ConfigFile::Status::FOUND: {
        LITR_INFO("Configuration file found under: {0}", config.GetFilePath());
        return EXIT_SUCCESS;
      }
    }
  }

  LITR_PROFILE_END_SESSION();

  return EXIT_SUCCESS;
}
