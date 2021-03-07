#pragma once

#include <string>

#include "Core.hpp"

namespace Litr {

class Application {
 public:
  Application();
  ExitStatus Run(int argc, char* argv[]);

 private:
  [[nodiscard]] static std::string SourceFromArguments(int argc, char* argv[]);

 private:
  Ref<Config::Loader> m_Config;
  Ref<ErrorHandler> m_ErrorHandler{CreateRef<ErrorHandler>()};

  std::string m_Source{};
  ExitStatus m_ExitStatus{ExitStatus::SUCCESS};
};

}  // namespace Litr
