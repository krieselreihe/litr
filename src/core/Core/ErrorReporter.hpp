#pragma once

#include <vector>

#include "Core/Configuration.hpp"

namespace Litr {

class ErrorReporter {
 public:
  static void PrintErrors(const std::vector<Configuration::Error>& errors);
  static void PrintError(const Configuration::Error& error);
};

}  // namespace Litr
