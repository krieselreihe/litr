/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "TomlError.hpp"

#include <fmt/format.h>

#include <vector>

#include "Core/Debug/Instrumentor.hpp"
#include "Core/StringUtils.hpp"

namespace Litr::Error {

std::string TomlError::extract_message(const std::string& message, const std::string& error) {
  LITR_PROFILE_FUNCTION();

  if (is_duplicated_value_error(error)) {
    return extract_duplicated_value_error(error);
  }

  if (is_duplicated_table_error(error)) {
    return extract_duplicated_table_error(error);
  }

  return message;
}

bool TomlError::is_duplicated_value_error(const std::string& error) {
  LITR_PROFILE_FUNCTION();

  return error.find("value already exists") != std::string::npos;
}

std::string TomlError::extract_duplicated_value_error(const std::string& error) {
  LITR_PROFILE_FUNCTION();

  const std::string extract{StringUtils::replace(error, "[error] toml::insert_value: ", "")};

  // Reformat lines
  std::vector<std::string> lines{};
  std::string output{};
  StringUtils::split_into(extract, '\n', lines);

  for (size_t i{0}; i < lines.size(); ++i) {
    // First line without change
    if (i == 0) {
      output.append(fmt::format("{}\n ...\n", lines[i]));
      continue;
    }

    if (is_file_reference(lines[i])) {
      continue;
    }

    // Ignore last 3 lines
    if (lines.size() - 3 <= i) {
      break;
    }

    lines[i].erase(0, 1);
    output.append(lines[i]).append("\n");
  }

  return StringUtils::trim(output, '\n');
}

bool TomlError::is_duplicated_table_error(const std::string& error) {
  LITR_PROFILE_FUNCTION();

  return error.find("table already exists") != std::string::npos;
}

std::string TomlError::extract_duplicated_table_error(const std::string& error) {
  LITR_PROFILE_FUNCTION();

  const std::string extract{StringUtils::replace(error, "[error] toml::insert_value: ", "")};

  // Reformat lines
  std::vector<std::string> lines{};
  std::string output{};
  StringUtils::split_into(extract, '\n', lines);

  // First line without change
  output.append(fmt::format("{}\n ...\n", lines[0]));

  for (size_t i{1}; i < lines.size(); ++i) {
    if (is_file_reference(lines[i])) {
      continue;
    }

    // Ignore last 3 lines
    if (lines.size() - 3 <= i) {
      break;
    }

    lines[i].erase(0, 1);
    output.append(lines[i]).append("\n");
  }

  return StringUtils::trim(output, '\n');
}

bool TomlError::is_file_reference(const std::string& line) {
  return line.find(" --> ") != std::string::npos;
}

}  // namespace Litr::Error
