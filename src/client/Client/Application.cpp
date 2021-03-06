#include "Application.hpp"

#include <fmt/printf.h>

namespace Litr {

Application::Application() {
  LITR_PROFILE_FUNCTION();

  Path cwd{FileSystem::GetCurrentWorkingDirectory()};
  Config::FileResolver configPath{cwd};

  switch (configPath.GetStatus()) {
    case Config::FileResolver::Status::NOT_FOUND: {
      fmt::print("No configuration file found!\n");
      m_ExitStatus = ExitStatus::FAILURE;
    }
    case Config::FileResolver::Status::DUPLICATE: {
      fmt::print(
          "You defined both, litr.toml and .litr.toml in {0}."
          "This is probably an error and you only want one of them.\n",
          configPath.GetFileDirectory());
      m_ExitStatus = ExitStatus::FAILURE;
    }
    case Config::FileResolver::Status::FOUND: {
      fmt::print("Configuration file found under: {0}\n", configPath.GetFilePath());
    }
  }

  if (m_ExitStatus == ExitStatus::FAILURE) {
    return;
  }

  m_Config = CreateRef<Config::Loader>(m_ErrorHandler, configPath.GetFilePath());
}

Application::ExitStatus Application::Run(int argc, char* argv[]) {
  LITR_PROFILE_FUNCTION();

  fmt::print("Hello, Litr!\n");
  m_Source = SourceFromArguments(argc, argv);

  // @todo: Test simple parser output
  const auto instruction{CreateRef<CLI::Instruction>()};
  CLI::Parser parser{m_ErrorHandler, instruction, m_Source};

  // Print any current errors.
  if (m_ErrorHandler->HasErrors()) {
    ErrorReporter::PrintErrors(m_ErrorHandler->GetErrors());
  }

  // @todo: Quick and dirty interpreter
  size_t offset{0};
  while (offset < instruction->Count()) {
    const auto code{static_cast<CLI::Instruction::Code>(instruction->Read(offset++))};
    if (code == CLI::Instruction::Code::EXECUTE) {
      const std::byte index{instruction->Read(offset)};
      const CLI::Instruction::Value name{instruction->ReadConstant(index)};

      Config::Query query{m_Config};
      Ref<Config::Command> command{query.GetCommand(name)};

      if (command == nullptr) {
        LITR_ERROR("Command \"{}\" not found!", name);
        return ExitStatus::FAILURE;
      }

      for (auto&& script : command->Script) {
        CLI::Shell::Exec(script, [](const std::string& result) {
          fmt::print("{}", result);
        });
      }

      offset++;
    }
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
      Litr::Utils::SplitInto(argument, '=', parts);
      argument = parts[0].append("=\"").append(parts[1]).append("\"");
    }
    // !!! HACK ALERT END !!!

    source.append(" ").append(argument);
  }

  return source;
}

}  // namespace Litr
