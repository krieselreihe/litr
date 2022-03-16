/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Shell.hpp"

#include <cstdio>

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Log.hpp"

namespace litr::cli {

Shell::Result Shell::exec(const std::string& command) {
  LITR_PROFILE_FUNCTION();

  return Shell::exec(command, []([[maybe_unused]] const std::string& _buffer) {});
}

Shell::Result Shell::exec(const std::string& command, const Path& path) {
  LITR_PROFILE_FUNCTION();

  return Shell::exec(command, path, []([[maybe_unused]] const std::string& _buffer) {});
}

Shell::Result Shell::exec(const std::string& command, const Shell::ExecCallback& callback) {
  LITR_PROFILE_FUNCTION();

  return Shell::exec(command, Path(), callback);
}

Shell::Result Shell::exec(
    const std::string& command, const Path& path, const Shell::ExecCallback& callback) {
  LITR_PROFILE_FUNCTION();

  Result result{};
  std::string cmd{create_command_string(command, path)};

  LITR_CORE_TRACE("Executing command \"{}\"", cmd);

  FILE* stream{popen(cmd.c_str(), "r")};

  if (stream) {
    constexpr int max_buffer{256};
    char buffer[max_buffer];

    while (!feof(stream)) {
      if (fgets(buffer, max_buffer, stream) != nullptr) {
        result.message.append(buffer);
        callback(std::string(buffer));
      }
    }

    result.status = get_status_code(pclose(stream));
  }

  return result;
}

ExitStatus Shell::get_status_code(int stream_status) {
  return static_cast<ExitStatus>(stream_status / 256);
}

std::string Shell::create_command_string(const std::string& command, const Path& path) {
  LITR_PROFILE_FUNCTION();

  std::string main_command{command};
  main_command.append(" 2>&1");

  if (path.empty()) {
    return main_command;
  }

  // Change current working directory
  std::string cmd{create_cd_command(path)};
  cmd.append(" && ").append(main_command);

  // Reset current working directory
  Path return_path{};
  for (size_t i{0}; i < path.count(); ++i) {
    return_path = return_path.append(std::string(".."));
  }

  cmd.append(" && ").append(create_cd_command(return_path));

  return cmd;
}

std::string Shell::create_cd_command(const Path& path) {
  LITR_PROFILE_FUNCTION();

  std::string cmd{"cd "};
  cmd.append(path.to_string());
  return cmd;
}

}  // namespace litr::cli
