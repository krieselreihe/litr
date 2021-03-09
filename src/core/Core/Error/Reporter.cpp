#include "Reporter.hpp"

#include <fmt/color.h>
#include <fmt/format.h>

#include "Core/Debug/Instrumentor.hpp"

namespace Litr::Error {

static uint32_t CountDigits(uint32_t number) {
  LITR_PROFILE_FUNCTION();

  if (number < 10) return 1;
  uint32_t count{0};
  while (number > 0) {
    number /= 10;
    count++;
  }
  return count;
}

void Reporter::PrintErrors(const std::vector<BaseError>& errors) {
  LITR_PROFILE_FUNCTION();

  for (const BaseError& error : errors) {
    PrintError(error);
  }
}

void Reporter::PrintError(const BaseError& error) {
  LITR_PROFILE_FUNCTION();

  switch (error.Type) {
    case BaseError::ErrorType::MALFORMED_FILE:
    case BaseError::ErrorType::MALFORMED_COMMAND:
    case BaseError::ErrorType::MALFORMED_PARAM:
    case BaseError::ErrorType::MALFORMED_SCRIPT:
    case BaseError::ErrorType::RESERVED_PARAM:
    case BaseError::ErrorType::UNKNOWN_COMMAND_PROPERTY:
    case BaseError::ErrorType::UNKNOWN_PARAM_VALUE:
    case BaseError::ErrorType::PARSER: {
      fmt::print(
          fg(fmt::color::crimson),
          "Error: {}\n{:d} | {}\n{:>{}} | {:>{}}{}\n",
          error.Description, error.Line, error.LineStr, " ",
          CountDigits(error.Line), "^ - ", error.Column, error.Message);
      break;
    }
    case BaseError::ErrorType::COMMAND_NOT_FOUND:
    case BaseError::ErrorType::EXECUTION_FAILURE: {
      fmt::print(fg(fmt::color::crimson), "Error: {}\n", error.Message);
      break;
    }
  }
}

}  // namespace Litr::Error
