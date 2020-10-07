#include <filesystem>
#include <iostream>

#include "Core.hpp"

static Litr::Path GetCurrentWorkingDirectory() {
  return std::filesystem::current_path();
}

int main() {
  LITR_PROFILE_BEGIN_SESSION_WITH_FILE("Litr", "litr-profile.json");

  std::cout << "Hello, Litr!\n";

  Litr::Path cwd{GetCurrentWorkingDirectory()};
  Litr::ConfigFile config{cwd};

  switch (config.GetStatus()) {
    case Litr::ConfigFile::Status::NOT_FOUND: {
      std::cout << "No configuration file found!\n";
      return EXIT_FAILURE;
    }
    case Litr::ConfigFile::Status::DUPLICATE: {
      std::cout << "You defined both, litr.toml and .litr.toml in " << config.GetFileDirectory()
                << " This is probably an error and you only want one of them.\n";
      return EXIT_FAILURE;
    }
    case Litr::ConfigFile::Status::FOUND: {
      std::cout << "Configuration file found under: " << config.GetFilePath() << "\n";
      return EXIT_SUCCESS;
    }
  }

  LITR_PROFILE_END_SESSION();

  return EXIT_SUCCESS;
}
