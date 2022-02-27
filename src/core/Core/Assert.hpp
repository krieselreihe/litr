/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#if LITR_ENABLE_ASSERTS
#include <csignal>

#include "Core/Log.hpp"
#define LITR_ASSERT(x, ...)                                  \
  {                                                          \
    if (!(x)) {                                              \
      LITR_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); \
      raise(SIGTRAP);                                        \
    }                                                        \
  }
#else
#define LITR_ASSERT(x, ...)
#endif
