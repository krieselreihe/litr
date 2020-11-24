#pragma once

#include <string>

#include "Core/Errors/Error.hpp"

namespace Litr {

class ErrorHandler {
 public:
  virtual ~ErrorHandler() = default;
  virtual void Push(const Error& err);

  [[nodiscard]] inline std::vector<Error> GetErrors() const { return m_Errors; }
  [[nodiscard]] inline bool HasErrors() const { return !m_Errors.empty(); };

  [[nodiscard]] static std::string GetTypeDescription(const Error& err);

 private:
  std::vector<Error> m_Errors{};
};

}  // namespace Litr
