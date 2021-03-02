#pragma once

#include <string>

#include "Core.hpp"

namespace Litr {

class Application {
 public:
  Application();
  int Run(int argc, char* argv[]);

 private:
  Ref<Config::Loader> m_Config;
  Ref<ErrorHandler> m_ErrorHandler{CreateRef<ErrorHandler>()};

  std::string m_Source{};
  int m_ExitStatus{EXIT_SUCCESS};
};

}  // namespace Litr
