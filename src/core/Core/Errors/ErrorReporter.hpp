#pragma once

#include <vector>

#include "Core/Errors/Error.hpp"

namespace Litr {

class ErrorReporter {
 public:
  static void PrintErrors(const std::vector<Error>& errors);
  static void PrintError(const Error& error);
};

}  // namespace Litr
