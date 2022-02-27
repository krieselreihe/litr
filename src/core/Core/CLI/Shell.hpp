/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>
#include <functional>

#include "Core/Base.hpp"
#include "Core/FileSystem.hpp"

namespace litr::CLI {

class Shell {
 public:
  struct Result {
    ExitStatus Status{ExitStatus::SUCCESS};
    std::string Message{};
  };

  using ExecCallback = std::function<void(const std::string&)>;

  static Result Exec(const std::string& command);
  static Result Exec(const std::string& command, const Path& path);
  static Result Exec(const std::string& command, const Shell::ExecCallback& callback);
  static Result Exec(const std::string& command, const Path& path, const Shell::ExecCallback& callback);

 private:
  [[nodiscard]] static ExitStatus GetStatusCode(int streamStatus);
  [[nodiscard]] static std::string CreateCommandString(const std::string& command, const Path& path);
  [[nodiscard]] static std::string CreateCdCommand(const Path& path);
};

}  // namespace litr::CLI
