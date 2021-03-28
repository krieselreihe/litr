#include "Handler.hpp"

#include "Core/Debug/Instrumentor.hpp"

namespace Litr::Error {

void Handler::Push(const BaseError& err) {
  LITR_PROFILE_FUNCTION();

  Get().m_Errors.push_back(err);
}

void Handler::Flush() {
  LITR_PROFILE_FUNCTION();

  Get().m_Errors.clear();
}

Handler::Errors Handler::GetErrors() {
  LITR_PROFILE_FUNCTION();

  return Get().m_Errors;
}

bool Handler::HasErrors() {
  LITR_PROFILE_FUNCTION();

  return !Get().m_Errors.empty();
}

}  // namespace Litr::Error
