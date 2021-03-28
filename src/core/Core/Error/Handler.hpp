#pragma once

#include <string>

#include "Core/Error/BaseError.hpp"

namespace Litr::Error {

class Handler {
 public:
  using Errors = std::vector<BaseError>;

  Handler(const Handler&) = delete;
  Handler& operator=(const Handler&) = delete;

  static void Push(const BaseError& err);
  static void Flush();

  [[nodiscard]] static Errors GetErrors();
  [[nodiscard]] static bool HasErrors();

 private:
  Handler() = default;

  static Handler& Get() {
    static Handler instance{};
    return instance;
  }

 private:
  Errors m_Errors{};
};

}  // namespace Litr::Error
