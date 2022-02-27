/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>

namespace Litr {

class TomlError {
 public:
  static std::string ExtractMessage(const std::string& message, const std::string& error);

 private:
  static bool IsDuplicatedValueError(const std::string& error);
  static std::string ExtractDuplicatedValueError(const std::string& error);

  static bool IsDuplicatedTableError(const std::string& error);
  static std::string ExtractDuplicatedTableError(const std::string& error);

  static bool IsFileReference(const std::string& line);
};

}  // namespace Litr
