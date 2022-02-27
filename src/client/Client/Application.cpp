/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Application.hpp"

#include <fmt/color.h>
#include <fmt/format.h>
#include <vector>

#include "Hooks/Handler.hpp"
#include "Hooks/Help.hpp"
#include "Hooks/Version.hpp"

namespace litr {

// @todo: Error handling here is horrible. In need of refactoring!
ExitStatus Application::Run(int argc, char* argv[]) {
  LITR_PROFILE_FUNCTION();

  const std::string source{source_from_arguments(argc, argv)};
  const auto instruction{CreateRef<CLI::Instruction>()};
  const CLI::Parser parser{instruction, source};

  // Litr called without any arguments:
  if (instruction->Count() == 0) {
    fmt::print("You can run `litr --help` to see what you can do here.\n");
    return ExitStatus::FAILURE;
  }

  Hook::Handler hooks{instruction};

  // Hooks before config
  hooks.Add(CLI::Instruction::Code::DEFINE, {"version", "v"}, Hook::Version::Print);
  if (hooks.Execute()) {
    return ExitStatus::SUCCESS;
  }

  const Path configPath{get_config_path()};
  if (m_exit_status == ExitStatus::FAILURE) {
    return m_exit_status;
  }

  Error::Reporter errorReporter{configPath};
  if (Error::Handler::HasErrors()) {
    errorReporter.PrintErrors(Error::Handler::GetErrors());
    return ExitStatus::FAILURE;
  }

  const auto config{CreateRef<Config::Loader>(configPath)};
  const auto interpreter{CreateRef<CLI::Interpreter>(instruction, config)};

  hooks.Add(CLI::Instruction::Code::DEFINE, {"help", "h"}, [&config](const Ref<CLI::Instruction>& instruction) {
    const Hook::Help help{config};
    help.Print(instruction);
  });
  if (hooks.Execute()) {
    return ExitStatus::SUCCESS;
  }

  // Run
  interpreter->Execute();
  if (Error::Handler::HasErrors()) {
    errorReporter.PrintErrors(Error::Handler::GetErrors());
    return ExitStatus::FAILURE;
  }

  return m_exit_status;
}

Path Application::get_config_path() {
  LITR_PROFILE_FUNCTION();

  Path cwd{FileSystem::GetCurrentWorkingDirectory()};
  Config::FileResolver configPath{cwd};

  switch (configPath.GetStatus()) {
    case Config::FileResolver::Status::NOT_FOUND: {
      fmt::print(fg(fmt::color::crimson), "No configuration file found!\n");
      m_exit_status = ExitStatus::FAILURE;
      break;
    }
    case Config::FileResolver::Status::DUPLICATE: {
      fmt::print(
          fg(fmt::color::gold),
          "You defined both, litr.toml and .litr.toml in {}. "
          "This is probably an error and you only want one of them.\n",
          configPath.GetFileDirectory());
      m_exit_status = ExitStatus::FAILURE;
      break;
    }
    case Config::FileResolver::Status::FOUND: {
      LITR_TRACE("Configuration file found under: {}\n", configPath.GetFilePath());
      break;
    }
  }

  return configPath.GetFilePath();
}

std::string Application::source_from_arguments(int argc, char** argv) {
  LITR_PROFILE_FUNCTION();

  std::string source{};

  // Start with 1 to skip the program name.
  for (int i{1}; i < argc; ++i) {
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

}  // namespace litr
