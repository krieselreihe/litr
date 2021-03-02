#include "Application.hpp"

#include <fmt/printf.h>

namespace Litr {

Application::Application() {
  Path cwd{FileSystem::GetCurrentWorkingDirectory()};
  Config::FileResolver configPath{cwd};

  switch (configPath.GetStatus()) {
    case Config::FileResolver::Status::NOT_FOUND: {
      fmt::print("No configuration file found!\n");
      m_ExitStatus = EXIT_FAILURE;
    }
    case Config::FileResolver::Status::DUPLICATE: {
      fmt::print(
          "You defined both, litr.toml and .litr.toml in {0}."
          "This is probably an error and you only want one of them.\n",
          configPath.GetFileDirectory());
      m_ExitStatus = EXIT_FAILURE;
    }
    case Config::FileResolver::Status::FOUND: {
      fmt::print("Configuration file found under: {0}\n", configPath.GetFilePath());
    }
  }

  if (m_ExitStatus == EXIT_FAILURE) {
    return;
  }

  m_Config = CreateRef<Config::Loader>(m_ErrorHandler, configPath.GetFilePath());
}

int Application::Run(int argc, char* argv[]) {
  LITR_PROFILE_FUNCTION();
  fmt::print("Hello, Litr!\n");

  // Create source string from arguments. Start with 1 to skip the program name.
  for (int i = 1; i < argc; ++i) {
    // !!! HACK ALERT START !!!
    // @todo: https://github.com/krieselreihe/litr/issues/20
    std::string argument{argv[i]};
    std::size_t found{argument.find('=')};

    if (found != std::string::npos) {
      std::vector<std::string> parts{};
      Litr::Utils::SplitInto(argument, '=', parts);
      argument = parts[0].append("=\"").append(parts[1]).append("\"");
    }
    // !!! HACK ALERT END !!!

    m_Source.append(" ").append(argument);
  }

  // @todo: Test simple parser output
  const auto instruction{CreateRef<CLI::Instruction>()};
  CLI::Parser parser{m_ErrorHandler, instruction, m_Source};

  // Print any current errors.
  if (m_ErrorHandler->HasErrors()) {
    ErrorReporter::PrintErrors(m_ErrorHandler->GetErrors());
  }

#ifdef SOME_TEST_CODE
  // @todo: Test output.

  for (auto&& command : m_Config->GetCommands()) {
    LITR_INFO("{}", *command);
  }

  LITR_INFO("========================================");
  auto testCommand{m_Config->GetCommand("hello")};
  if (testCommand != nullptr) {
    LITR_INFO("TEST COMMAND: {}", testCommand->Script[0]);
    Shell::Result result{Shell::Exec(testCommand->Script[0])};
    LITR_INFO("OUTPUT (Status code {}): {}", result.Status, result.Message);
  }
#endif

  return m_ExitStatus;
}

}  // namespace Litr
