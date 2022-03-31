/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>
#include <vector>

#include "Core/Error/BaseError.hpp"

namespace Litr::Error {

class Handler {
 public:
  using Errors = std::vector<BaseError>;

  Handler(const Handler&) = delete;
  Handler(const Handler&&) = delete;
  Handler& operator=(const Handler&) = delete;
  Handler& operator=(const Handler&&) = delete;

  ~Handler() = default;

  static void push(const BaseError& err);
  static void flush();

  [[nodiscard]] static Errors get_errors();
  [[nodiscard]] static bool has_errors();

 private:
  Handler() = default;

  static Handler& get() {
    static Handler instance{};
    return instance;
  }

  Errors m_errors{};
};

}  // namespace Litr::Error
