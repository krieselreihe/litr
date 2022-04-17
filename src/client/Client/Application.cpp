/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Application.hpp"

#include <fmt/color.h>
#include <fmt/format.h>

#include <memory>
#include <vector>

#include "Hooks/Handler.hpp"
#include "Hooks/Help/NewHelp.hpp"
#include "Hooks/Version.hpp"

namespace Litr {

ExitStatus Application::run(const std::vector<std::string>& arguments) {
  LITR_PROFILE_FUNCTION();

  const std::string source{source_from_arguments(arguments)};
  const auto instruction{std::make_shared<CLI::Instruction>()};
  const CLI::Parser parser{instruction, source};

  // Litr called without any arguments:
  if (instruction->count() == 0) {
    fmt::print("You can run `litr --help` to see what you can do here.\n");
    return ExitStatus::FAILURE;
  }

  Hook::Handler hooks{instruction};

  // Hooks before config
  hooks.add(CLI::Instruction::Code::DEFINE, {"version", "v"}, []() {
    fmt::print(Hook::Version::read());
  });
  if (hooks.execute()) {
    return ExitStatus::SUCCESS;
  }

  const Path config_path{get_config_path()};
  if (m_exit_status == ExitStatus::FAILURE) {
    return m_exit_status;
  }

  Error::Reporter error_reporter{config_path};
  if (Error::Handler::has_errors()) {
    error_reporter.print_errors(Error::Handler::errors());
    return ExitStatus::FAILURE;
  }

  const auto config{std::make_shared<Config::Loader>(config_path)};
  const auto interpreter{std::make_shared<CLI::Interpreter>(instruction, config)};

  Hook::NewHelp help{config, instruction};
  hooks.add(CLI::Instruction::Code::DEFINE, {"help", "h"}, [&help]() {
    fmt::print(help.read());
  });
  if (hooks.execute()) {
    return ExitStatus::SUCCESS;
  }

  // Run
  interpreter->execute();
  if (Error::Handler::has_errors()) {
    error_reporter.print_errors(Error::Handler::errors());
    return ExitStatus::FAILURE;
  }

  return m_exit_status;
}

Path Application::get_config_path() {
  LITR_PROFILE_FUNCTION();

  const Path cwd{FileSystem::current_working_directory()};
  const Config::FileResolver config_path{cwd};

  switch (config_path.status()) {
    case Config::FileResolver::Status::NOT_FOUND: {
      fmt::print(fg(fmt::color::crimson), "No configuration file found!\n");
      m_exit_status = ExitStatus::FAILURE;
      break;
    }
    case Config::FileResolver::Status::DUPLICATE: {
      fmt::print(fg(fmt::color::gold),
          "You defined both, litr.toml and .litr.toml in {}. "
          "This is probably an error and you only want one of them.\n",
          config_path.file_directory());
      m_exit_status = ExitStatus::FAILURE;
      break;
    }
    case Config::FileResolver::Status::FOUND: {
      LITR_TRACE("Configuration file found under: {}\n", config_path.file_path());
      break;
    }
  }

  return config_path.file_path();
}

std::string Application::source_from_arguments(const std::vector<std::string>& arguments) {
  LITR_PROFILE_FUNCTION();

  std::string source{};

  // !!! HACK ALERT START !!!
  // @todo: https://github.com/krieselreihe/litr/issues/20
  for (std::string argument : arguments) {
    const std::size_t found{argument.find('=')};

    if (found != std::string::npos) {
      std::vector<std::string> parts{};
      StringUtils::split_into(argument, '=', parts);
      argument = parts[0].append("=\"").append(parts[1]).append("\"");
    }

    source.append(" ").append(argument);
  }
  // !!! HACK ALERT END !!!

  return source;
}

}  // namespace Litr
