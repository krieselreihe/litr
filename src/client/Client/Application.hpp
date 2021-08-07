#pragma once

#include <string>

#include "Core.hpp"

namespace Litr {

class Application {
 public:
  Application() = default;
  ExitStatus Run(int argc, char* argv[]);

 private:
  [[nodiscard]] Path GetConfigPath();
  [[nodiscard]] static std::string SourceFromArguments(int argc, char* argv[]);

 private:
  ExitStatus m_ExitStatus{ExitStatus::SUCCESS};
};

}  // namespace Litr
