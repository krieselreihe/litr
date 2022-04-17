/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Handler.hpp"

#include "Core/Debug/Instrumentor.hpp"

namespace Litr::Error {

void Handler::push(const BaseError& err) {
  LITR_PROFILE_FUNCTION();

  get().m_errors.push_back(err);
}

void Handler::flush() {
  LITR_PROFILE_FUNCTION();

  get().m_errors.clear();
}

Handler::Errors Handler::errors() {
  LITR_PROFILE_FUNCTION();

  return get().m_errors;
}

bool Handler::has_errors() {
  LITR_PROFILE_FUNCTION();

  return !get().m_errors.empty();
}

}  // namespace Litr::Error
