#include "ErrorReporter.hpp"

#include <fmt/format.h>
#include <fmt/color.h>

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Errors/ErrorHandler.hpp"

namespace Litr {

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

void ErrorReporter::PrintErrors(const std::vector<Error>& errors) {
  LITR_PROFILE_FUNCTION();

  for (const Error& error : errors) {
    PrintError(error);
  }
}

void ErrorReporter::PrintError(const Error& error) {
  LITR_PROFILE_FUNCTION();

  switch (error.Type) {
    case ErrorType::MALFORMED_FILE:
    case ErrorType::MALFORMED_COMMAND:
    case ErrorType::MALFORMED_PARAM:
    case ErrorType::MALFORMED_SCRIPT:
    case ErrorType::RESERVED_PARAM:
    case ErrorType::UNKNOWN_COMMAND_PROPERTY:
    case ErrorType::UNKNOWN_PARAM_VALUE:
    case ErrorType::PARSER_ERROR: {
      const std::string type{ErrorHandler::GetTypeDescription(error)};
      fmt::print(
          fg(fmt::color::crimson),
          "Error: {}\n{:d} | {}\n{:>{}} | {:>{}}{}\n",
          type, error.Line, error.LineStr, " ",
          CountDigits(error.Line), "^ - ", error.Column, error.Message);
      break;
    }
    case ErrorType::COMMAND_NOT_FOUND:
    case ErrorType::EXECUTION_FAILURE: {
      fmt::print(fg(fmt::color::crimson), "Error: {}\n", error.Message);
      break;
    }
  }
}

}  // namespace Litr
