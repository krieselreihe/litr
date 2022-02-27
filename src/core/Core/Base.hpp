/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <memory>
#include <utility>

namespace litr {

template <typename T>
using Scope = std::unique_ptr<T>;

template <typename T, typename... Args>
constexpr Scope<T> create_scope(Args&&... args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Ref = std::shared_ptr<T>;

template <typename T, typename... Args>
constexpr Ref<T> create_ref(Args&&... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

enum class ExitStatus : int {
  SUCCESS = 0,
  FAILURE = 1
};

}  // namespace litr
