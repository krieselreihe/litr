#pragma once

#include <string>

#include "Core/Errors/Error.hpp"

namespace Litr {

class ErrorHandler {
 public:
  ErrorHandler(const ErrorHandler&) = delete;
  ErrorHandler& operator=(const ErrorHandler&) = delete;

  static void Push(const Error& err);
  static void Flush();

  [[nodiscard]] static std::vector<Error> GetErrors();
  [[nodiscard]] static bool HasErrors();

  [[nodiscard]] static std::string GetTypeDescription(const Error& err);

 private:
  ErrorHandler() = default;

  static ErrorHandler& Get() {
    static ErrorHandler instance{};
    return instance;
  }

 private:
  std::vector<Error> m_Errors{};
};

}  // namespace Litr
