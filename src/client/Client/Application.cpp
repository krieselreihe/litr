#include "Application.hpp"

#include <fmt/color.h>
#include <fmt/format.h>

#include "Hooks/Help.hpp"
#include "Hooks/Version.hpp"

namespace Litr {

Application::Application() {
  LITR_PROFILE_FUNCTION();

  LITR_TRACE("Hello, Litr!\n");

  Path configPath{GetConfigPath()};
  if (m_ExitStatus == ExitStatus::FAILURE) {
    return;
  }

  m_Config = CreateRef<Config::Loader>(configPath);
}

ExitStatus Application::Run(int argc, char* argv[]) {
  LITR_PROFILE_FUNCTION();

  if (m_ExitStatus == ExitStatus::FAILURE) {
    return m_ExitStatus;
  }

  m_Source = SourceFromArguments(argc, argv);

  const auto instruction{CreateRef<CLI::Instruction>()};
  const CLI::Parser parser{instruction, m_Source};
  Error::Reporter errorReporter{m_Config};

  // Print parser errors if any:
  if (Error::Handler::HasErrors()) {
    errorReporter.PrintErrors(Error::Handler::GetErrors());
    return ExitStatus::FAILURE;
  }

  // Litr called without any arguments:
  if (instruction->Count() == 0) {
    fmt::print("You can run `litr --help` to see what you can do here.\n");
    return ExitStatus::FAILURE;
  }

  const auto interpreter{CreateRef<CLI::Interpreter>(instruction, m_Config)};

  const Help help{m_Config};
  const std::vector<std::string> versionParams{"version", "v"};
  const std::vector<std::string> helpParams{"help", "h"};

  interpreter->AddHook(CLI::Instruction::Code::DEFINE, versionParams, Version::Print);
  interpreter->AddHook(
      CLI::Instruction::Code::DEFINE, helpParams,
      [&help](const Ref<CLI::Instruction>& instruction) {
        help.Print(instruction);
      });

  interpreter->Execute();

  // Print interpreter errors if any:
  if (Error::Handler::HasErrors()) {
    errorReporter.PrintErrors(Error::Handler::GetErrors());
    return ExitStatus::FAILURE;
  }

  return m_ExitStatus;
}

Path Application::GetConfigPath() {
  LITR_PROFILE_FUNCTION();

  Path cwd{FileSystem::GetCurrentWorkingDirectory()};
  Config::FileResolver configPath{cwd};

  switch (configPath.GetStatus()) {
    case Config::FileResolver::Status::NOT_FOUND: {
      fmt::print(fg(fmt::color::crimson), "No configuration file found!\n");
      m_ExitStatus = ExitStatus::FAILURE;
      break;
    }
    case Config::FileResolver::Status::DUPLICATE: {
      fmt::print(
          fg(fmt::color::gold),
          "You defined both, litr.toml and .litr.toml in {}. "
          "This is probably an error and you only want one of them.\n",
          configPath.GetFileDirectory());
      m_ExitStatus = ExitStatus::FAILURE;
      break;
    }
    case Config::FileResolver::Status::FOUND: {
      LITR_TRACE("Configuration file found under: {}\n", configPath.GetFilePath());
      break;
    }
  }

  return configPath.GetFilePath();
}

std::string Application::SourceFromArguments(int argc, char** argv) {
  LITR_PROFILE_FUNCTION();

  std::string source{};

  // Start with 1 to skip the program name.
  for (int i = 1; i < argc; ++i) {
    // !!! HACK ALERT START !!!
    // @todo: https://github.com/krieselreihe/litr/issues/20
    std::string argument{argv[i]};
    std::size_t found{argument.find('=')};

    if (found != std::string::npos) {
      std::vector<std::string> parts{};
      Utils::SplitInto(argument, '=', parts);
      argument = parts[0].append("=\"").append(parts[1]).append("\"");
    }
    // !!! HACK ALERT END !!!

    source.append(" ").append(argument);
  }

  return source;
}

}  // namespace Litr
