#include "Application.hpp"

#include <fmt/printf.h>

namespace Litr {

Application::Application() {
  Path cwd{FileSystem::GetCurrentWorkingDirectory()};
  ConfigFileResolver configPath{cwd};

  switch (configPath.GetStatus()) {
    case ConfigFileResolver::Status::NOT_FOUND: {
      fmt::print("No configuration file found!\n");
      m_ExitStatus = EXIT_FAILURE;
    }
    case Litr::ConfigFileResolver::Status::DUPLICATE: {
      fmt::print(
          "You defined both, litr.toml and .litr.toml in {0}."
          "This is probably an error and you only want one of them.\n",
          configPath.GetFileDirectory());
      m_ExitStatus = EXIT_FAILURE;
    }
    case Litr::ConfigFileResolver::Status::FOUND: {
      fmt::print("Configuration file found under: {0}\n", configPath.GetFilePath());
    }
  }

  if (m_ExitStatus == EXIT_FAILURE) {
    return;
  }

  m_Config = CreateRef<ConfigLoader>(m_ErrorHandler, configPath.GetFilePath());
}

int Application::Run(int argc, char* argv[]) {
  LITR_PROFILE_FUNCTION();
  fmt::print("Hello, Litr!\n");

  // Create source string from arguments. Start with 1 to skip the program name.
  for (int i = 1; i < argc; ++i) {
    m_Source.append(" ").append(argv[i]);
  }

  // Print any current errors.
  if (m_ErrorHandler->HasErrors()) {
    ErrorReporter::PrintErrors(m_ErrorHandler->GetErrors());
  }

  // @todo: Test simple parser output
  Litr::Parser parser{m_Source};

#ifdef SOME_TEST_CODE
  // @todo: Test output.

  for (const auto& command : config->GetCommands()) {
    LITR_INFO("{}", *command);
  }

  LITR_INFO("========================================");
  auto testCommand{config->GetCommand("hello")};
  if (testCommand != nullptr) {
    LITR_INFO("TEST COMMAND: {}", testCommand->Script[0]);
    Litr::Shell::Result result{Litr::Shell::Exec(testCommand->Script[0])};
    LITR_INFO("OUTPUT (Status code {}): {}", result.Status, result.Message);
  }
#endif

  return m_ExitStatus;
}

}  // namespace Litr
