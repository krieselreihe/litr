#pragma once

#include <vector>

#include "Core/Error/BaseError.hpp"
#include "Core/Config/Loader.hpp"
#include "Core/FileSystem.hpp"

namespace Litr::Error {

class Reporter {
 public:
  explicit Reporter(const Ref<Config::Loader>& config);

  void PrintErrors(const std::vector<BaseError>& errors);
  void PrintError(const BaseError& error);

 private:
  static uint32_t CountDigits(uint32_t number);

  const Path m_FilePath;
  bool m_MultipleErrors{false};
};

}  // namespace Litr::Error
