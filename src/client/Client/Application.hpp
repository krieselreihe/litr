/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>

#include "Core.hpp"

namespace litr {

class Application {
 public:
  Application() = default;
  ExitStatus Run(int argc, char* argv[]);

 private:
  [[nodiscard]] Path get_config_path();
  [[nodiscard]] static std::string source_from_arguments(int argc, char* argv[]);

 private:
  ExitStatus m_exit_status{ExitStatus::SUCCESS};
};

}  // namespace litr
