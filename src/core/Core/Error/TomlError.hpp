/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>

namespace Litr::Error {

class TomlError {
 public:
  static std::string extract_message(const std::string& message, const std::string& error);

 private:
  static bool is_duplicated_value_error(const std::string& error);
  static std::string extract_duplicated_value_error(const std::string& error);

  static bool is_duplicated_table_error(const std::string& error);
  static std::string extract_duplicated_table_error(const std::string& error);

  static bool is_file_reference(const std::string& line);
};

}  // namespace Litr::Error
