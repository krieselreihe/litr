#pragma once

#if LITR_ENABLE_ASSERTS == 1
#include <csignal>
#include "Core/Log.hpp"
#define LITR_ASSERT(x, ...)                             \
  {                                                   \
    if (!(x)) {                                       \
      LITR_ERROR("Assertion failed: {0}", __VA_ARGS__); \
      raise(SIGTRAP);                                 \
    }                                                 \
  }
#else
#define LITR_ASSERT(x, ...)
#endif
