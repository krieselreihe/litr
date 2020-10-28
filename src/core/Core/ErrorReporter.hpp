#pragma once

#include <vector>

#include "Core/Errors.hpp"

namespace Litr {

class ErrorReporter {
 public:
  static void PrintErrors(const std::vector<ConfigurationError>& errors);
  static void PrintError(const ConfigurationError& error);
};

}  // namespace Litr
