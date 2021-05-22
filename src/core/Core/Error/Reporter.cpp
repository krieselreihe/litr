#include "Reporter.hpp"

#include <string>
#include <fmt/color.h>
// Defined to build on g++
#include <fmt/format.h>

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Utils.hpp"

namespace Litr::Error {

Reporter::Reporter(const Ref<Config::Loader>& config) : m_FilePath(config->GetFilePath()) {}

void Reporter::PrintErrors(const std::vector<BaseError>& errors) {
  LITR_PROFILE_FUNCTION();

  m_MultipleErrors = false;
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
    case BaseError::ErrorType::VALUE_ALREADY_IN_USE:
    case BaseError::ErrorType::UNKNOWN_COMMAND_PROPERTY:
    case BaseError::ErrorType::UNKNOWN_PARAM_VALUE:
    case BaseError::ErrorType::CLI_PARSER:
    case BaseError::ErrorType::SCRIPT_PARSER: {
      if (m_MultipleErrors) {
        fmt::print(fg(fmt::color::crimson), " ...\n");
      } else {
        // Title
        fmt::print(fg(fmt::color::crimson),"Error: {}\n", error.Description);
        // File
        fmt::print(fg(fmt::color::dark_gray),"  → {}\n", m_FilePath);
      }

      // Line view
      fmt::print(fg(fmt::color::crimson), "{:d} | {}\n", error.Location.Line, error.Location.LineStr);

      // Message
      fmt::print(fg(fmt::color::crimson), "{:>{}} | {:>{}}{}\n",
          " ", CountDigits(error.Location.Line), "└─ ", error.Location.Column, error.Message);
      break;
    }
    case BaseError::ErrorType::COMMAND_NOT_FOUND: {
      fmt::print(fg(fmt::color::crimson), "Error: {}\n", error.Message);
      break;
    }
    case BaseError::ErrorType::EXECUTION_FAILURE: {
      // Message
      fmt::print(fg(fmt::color::crimson), "Error: {}\n", error.Message);
      // File
      fmt::print(fg(fmt::color::dark_gray),"  → {}\n", m_FilePath);
      break;
    }
  }

  m_MultipleErrors = true;
}

uint32_t Reporter::CountDigits(uint32_t number) {
  LITR_PROFILE_FUNCTION();

  if (number < 10) return 1;
  uint32_t count{0};
  while (number > 0) {
    number /= 10;
    count++;
  }
  return count;
}

}  // namespace Litr::Error
