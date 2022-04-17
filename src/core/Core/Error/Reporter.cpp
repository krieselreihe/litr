/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Reporter.hpp"

#include <fmt/color.h>
// Defined to build on g++
#include <fmt/format.h>

#include <string>
#include <utility>

#include "Core/Debug/Instrumentor.hpp"
#include "Core/StringUtils.hpp"

namespace Litr::Error {

Reporter::Reporter(Path path) : m_file_path(std::move(path)) {}

void Reporter::print_errors(const std::vector<BaseError>& errors) {
  LITR_PROFILE_FUNCTION();

  m_multiple_errors = false;
  for (const BaseError& error : errors) {
    print_error(error);
  }
}

void Reporter::print_error(const BaseError& error) {
  LITR_PROFILE_FUNCTION();

  switch (error.type) {
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
      if (m_multiple_errors) {
        fmt::print(fg(fmt::color::crimson), " ...\n");
      } else {
        // Title
        fmt::print(fg(fmt::color::crimson), "Error: {}\n", error.description);
        // File
        fmt::print(fg(fmt::color::dark_gray), "  → {}\n", m_file_path);
      }

      // Line view
      fmt::print(
          fg(fmt::color::crimson), "{:d} | {}\n", error.location.line, error.location.line_str);

      // Message
      fmt::print(fg(fmt::color::crimson),
          "{:>{}} | {:>{}}{}\n",
          " ",
          count_digits(error.location.line),
          "└─ ",
          error.location.column,
          error.message);
      break;
    }
    case BaseError::ErrorType::COMMAND_NOT_FOUND: {
      fmt::print(fg(fmt::color::crimson), "Error: {}\n", error.message);
      break;
    }
    case BaseError::ErrorType::EXECUTION_FAILURE: {
      // Message
      fmt::print(fg(fmt::color::crimson), "Error: {}\n", error.message);
      // File
      fmt::print(fg(fmt::color::dark_gray), "  → {}\n", m_file_path);
      break;
    }
  }

  m_multiple_errors = true;
}

uint32_t Reporter::count_digits(uint32_t number) {
  LITR_PROFILE_FUNCTION();

  constexpr uint32_t base{10};
  if (number < base) {
    return 1;
  }
  uint32_t count{0};
  while (number > 0) {
    number /= base;
    ++count;
  }
  return count;
}

}  // namespace Litr::Error
