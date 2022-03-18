/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <functional>
#include <string>

#include "Core/ExitStatus.hpp"
#include "Core/FileSystem.hpp"

namespace litr::cli {

class Shell {
 public:
  struct Result {
    ExitStatus status{ExitStatus::SUCCESS};
    std::string message{};
  };

  using ExecCallback = std::function<void(const std::string&)>;

  static Result exec(const std::string& command, const Path& path);
  static Result exec(const std::string& command, const Shell::ExecCallback& callback);
  static Result exec(
      const std::string& command, const Path& path, const Shell::ExecCallback& callback);

 private:
  [[nodiscard]] static ExitStatus get_status_code(int stream_status);
  [[nodiscard]] static std::string create_command_string(
      const std::string& command, const Path& path);
  [[nodiscard]] static std::string create_cd_command(const Path& path);
};

}  // namespace litr::cli
