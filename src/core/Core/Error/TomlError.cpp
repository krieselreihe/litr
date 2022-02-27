/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "TomlError.hpp"

#include <fmt/format.h>

#include <vector>

#include "Core/Utils.hpp"
#include "Core/Debug/Instrumentor.hpp"

namespace litr {

std::string TomlError::ExtractMessage(const std::string& message, const std::string& error) {
  LITR_PROFILE_FUNCTION();

  if (IsDuplicatedValueError(error)) {
    return ExtractDuplicatedValueError(error);
  }

  if (IsDuplicatedTableError(error)) {
    return ExtractDuplicatedTableError(error);
  }

  return message;
}

bool TomlError::IsDuplicatedValueError(const std::string& error) {
  LITR_PROFILE_FUNCTION();

  return error.find("value already exists") != std::string::npos;
}

std::string TomlError::ExtractDuplicatedValueError(const std::string& error) {
  LITR_PROFILE_FUNCTION();

  const std::string extract{Utils::Replace(error, "[error] toml::insert_value: ", "")};

  // Reformat lines
  std::vector<std::string> lines{};
  std::string output{};
  Utils::SplitInto(extract, '\n', lines);

  for (size_t i{0}; i < lines.size(); ++i) {
    // First line without change
    if (i == 0) {
      output.append(fmt::format("{}\n ...\n", lines[i]));
      continue;
    }

    if (IsFileReference(lines[i])) continue;

    // Ignore last 3 lines
    if (lines.size() - 3 <= i) break;

    lines[i].erase(0, 1);
    output.append(lines[i]).append("\n");
  }

  return Utils::Trim(output, '\n');
}

bool TomlError::IsDuplicatedTableError(const std::string& error) {
  LITR_PROFILE_FUNCTION();

  return error.find("table already exists") != std::string::npos;
}

std::string TomlError::ExtractDuplicatedTableError(const std::string& error) {
  LITR_PROFILE_FUNCTION();

  const std::string extract{Utils::Replace(error, "[error] toml::insert_value: ", "")};

  // Reformat lines
  std::vector<std::string> lines{};
  std::string output{};
  Utils::SplitInto(extract, '\n', lines);

  for (size_t i{0}; i < lines.size(); ++i) {
    // First line without change
    if (i == 0) {
      output.append(fmt::format("{}\n ...\n", lines[i]));
      continue;
    }

    if (IsFileReference(lines[i])) continue;

    // Ignore last 3 lines
    if (lines.size() - 3 <= i) break;

    lines[i].erase(0, 1);
    output.append(lines[i]).append("\n");
  }

  return Utils::Trim(output, '\n');
}

bool TomlError::IsFileReference(const std::string& line) {
  return line.find(" --> ") != std::string::npos;
}

}  // namespace litr
