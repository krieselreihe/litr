#include "ErrorReporter.hpp"

#include <fmt/format.h>
#include <fmt/color.h>

#include "Core/Debug/Instrumentor.hpp"

namespace Litr {

static uint32_t countDigits(uint32_t number) {
  LITR_PROFILE_FUNCTION();

  if (number < 10) return 1;
  uint32_t count{0};
  while (number > 0) {
    number /= 10;
    count++;
  }
  return count;
}

void ErrorReporter::PrintErrors(const std::vector<Configuration::Error>& errors) {
  LITR_PROFILE_FUNCTION();

  for (auto& error : errors) {
    PrintError(error);
  }
}

void ErrorReporter::PrintError(const Configuration::Error& error) {
  LITR_PROFILE_FUNCTION();

  std::string type{Configuration::Error::GetTypeDescription(error)};
  fmt::print(
      fg(fmt::color::crimson),
      "Error: {}\n{:d} | {}\n{:>{}} | {:>{}}{}\n",
      type, error.Line, error.LineStr, " ", countDigits(error.Line), " ^ - ", error.Column, error.Message);
}

}  // namespace Litr
