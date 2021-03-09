#include "Application.hpp"

#include <fmt/color.h>

namespace Litr {

Application::Application() {
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
          "You defined both, litr.toml and .litr.toml in {0}. "
          "This is probably an error and you only want one of them.\n",
          configPath.GetFileDirectory());
      m_ExitStatus = ExitStatus::FAILURE;
      break;
    }
    case Config::FileResolver::Status::FOUND: {
      fmt::print("Configuration file found under: {0}\n", configPath.GetFilePath());
      break;
    }
  }

  if (m_ExitStatus == ExitStatus::FAILURE) {
    return;
  }

  m_Config = CreateRef<Config::Loader>(configPath.GetFilePath());
}

ExitStatus Application::Run(int argc, char* argv[]) {
  LITR_PROFILE_FUNCTION();

  if (m_ExitStatus == ExitStatus::FAILURE) {
    return m_ExitStatus;
  }

  fmt::print("Hello, Litr!\n");
  m_Source = SourceFromArguments(argc, argv);

  const auto instruction{CreateRef<CLI::Instruction>()};
  const CLI::Parser parser{instruction, m_Source};

  // Print parser errors if any:
  if (Error::Handler::HasErrors()) {
    Error::Reporter::PrintErrors(Error::Handler::GetErrors());
    return ExitStatus::FAILURE;
  }

  CLI::Interpreter interpreter{instruction, m_Config};
  interpreter.Execute([](const std::string& result) {
    fmt::print("{}", result);
  });

  // Print interpreter errors if any:
  if (Error::Handler::HasErrors()) {
    Error::Reporter::PrintErrors(Error::Handler::GetErrors());
    return ExitStatus::FAILURE;
  }

  return m_ExitStatus;
}

std::string Application::SourceFromArguments(int argc, char** argv) {
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
