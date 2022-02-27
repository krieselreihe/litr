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
ExitStatus Application::run(int argc, char* argv[]) {
  LITR_PROFILE_FUNCTION();

  const std::string source{source_from_arguments(argc, argv)};
  const auto instruction{create_ref<cli::Instruction>()};
  const cli::Parser parser{instruction, source};

  // Litr called without any arguments:
  if (instruction->count() == 0) {
    fmt::print("You can run `litr --help` to see what you can do here.\n");
    return ExitStatus::FAILURE;
  }

  hook::Handler hooks{instruction};

  // Hooks before config
  hooks.add(cli::Instruction::Code::DEFINE, {"version", "v"}, hook::Version::print);
  if (hooks.execute()) {
    return ExitStatus::SUCCESS;
  }

  const Path config_path{get_config_path()};
  if (m_exit_status == ExitStatus::FAILURE) {
    return m_exit_status;
  }

  error::Reporter error_reporter{config_path};
  if (error::Handler::has_errors()) {
    error_reporter.print_errors(error::Handler::get_errors());
    return ExitStatus::FAILURE;
  }

  const auto config{create_ref<config::Loader>(config_path)};
  const auto interpreter{create_ref<cli::Interpreter>(instruction, config)};

  hooks.add(cli::Instruction::Code::DEFINE, {"help", "h"}, [&config](const Ref<cli::Instruction>& instruction) {
    const hook::Help help{config};
    help.print(instruction);
  });
  if (hooks.execute()) {
    return ExitStatus::SUCCESS;
  }

  // Run
  interpreter->execute();
  if (error::Handler::has_errors()) {
    error_reporter.print_errors(error::Handler::get_errors());
    return ExitStatus::FAILURE;
  }

  return m_exit_status;
}

Path Application::get_config_path() {
  LITR_PROFILE_FUNCTION();

  Path cwd{FileSystem::get_current_working_directory()};
  config::FileResolver config_path{cwd};

  switch (config_path.get_status()) {
    case config::FileResolver::Status::NOT_FOUND: {
      fmt::print(fg(fmt::color::crimson), "No configuration file found!\n");
      m_exit_status = ExitStatus::FAILURE;
      break;
    }
    case config::FileResolver::Status::DUPLICATE: {
      fmt::print(
          fg(fmt::color::gold),
          "You defined both, litr.toml and .litr.toml in {}. "
          "This is probably an error and you only want one of them.\n",
          config_path.get_file_directory());
      m_exit_status = ExitStatus::FAILURE;
      break;
    }
    case config::FileResolver::Status::FOUND: {
      LITR_TRACE("Configuration file found under: {}\n", config_path.get_file_path());
      break;
    }
  }

  return config_path.get_file_path();
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
      utils::split_into(argument, '=', parts);
      argument = parts[0].append("=\"").append(parts[1]).append("\"");
    }
    // !!! HACK ALERT END !!!

    source.append(" ").append(argument);
  }

  return source;
}

}  // namespace litr
