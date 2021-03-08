#pragma once

#include <vector>

#include "Core/Error/BaseError.hpp"

namespace Litr::Error {

class Reporter {
 public:
  static void PrintErrors(const std::vector<BaseError>& errors);
  static void PrintError(const BaseError& error);
};

}  // namespace Litr::Error
