#pragma once

#include <vector>

#include "Core/Errors/ConfigurationError.hpp"

namespace Litr {

class ErrorReporter {
 public:
  static void PrintErrors(const std::vector<ConfigurationError>& errors);
  static void PrintError(const ConfigurationError& error);
};

}  // namespace Litr
