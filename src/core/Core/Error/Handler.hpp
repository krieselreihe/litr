#pragma once

#include <string>

#include "Core/Error/BaseError.hpp"

namespace Litr::Error {

class Handler {
 public:
  Handler(const Handler&) = delete;
  Handler& operator=(const Handler&) = delete;

  static void Push(const BaseError& err);
  static void Flush();

  [[nodiscard]] static std::vector<BaseError> GetErrors();
  [[nodiscard]] static bool HasErrors();

 private:
  Handler() = default;

  static Handler& Get() {
    static Handler instance{};
    return instance;
  }

 private:
  std::vector<BaseError> m_Errors{};
};

}  // namespace Litr::Error
