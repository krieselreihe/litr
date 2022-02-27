/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#include "Core/Base.hpp"

namespace litr {

class Log {
 public:
  Log(const Log&) = delete;
  Log& operator=(const Log&) = delete;

  static Ref<spdlog::logger>& GetCoreLogger() {
    return Get().s_CoreLogger;
  }

  static Ref<spdlog::logger>& GetClientLogger() {
    return Get().s_ClientLogger;
  }

 private:
  // The constructor shall not be deleted but used to bootstrap the logger. Ignoring
  // the lint warning is ignoring doing `Log() = delete`.
  // NOLINTNEXTLINE
  Log();

  static Log& Get() {
    static Log instance{};
    return instance;
  }

  Ref<spdlog::logger> s_CoreLogger;
  Ref<spdlog::logger> s_ClientLogger;
};

}  // namespace litr

// Core logging

#ifndef LITR_DEACTIVATE_LOGGING

#if DEBUG
#define LITR_CORE_TRACE(...) ::litr::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LITR_CORE_DEBUG(...) ::litr::Log::GetCoreLogger()->debug(__VA_ARGS__)
#else
#define LITR_CORE_TRACE(...)
#define LITR_CORE_DEBUG(...)
#endif

#define LITR_CORE_INFO(...) ::litr::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LITR_CORE_WARN(...) ::litr::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LITR_CORE_ERROR(...) ::litr::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LITR_CORE_FATAL(...) ::litr::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client logging

#if DEBUG
#define LITR_TRACE(...) ::litr::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LITR_DEBUG(...) ::litr::Log::GetClientLogger()->debug(__VA_ARGS__)
#else
#define LITR_TRACE(...)
#define LITR_DEBUG(...)
#endif

#define LITR_INFO(...) ::litr::Log::GetClientLogger()->info(__VA_ARGS__)
#define LITR_WARN(...) ::litr::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LITR_ERROR(...) ::litr::Log::GetClientLogger()->error(__VA_ARGS__)
#define LITR_FATAL(...) ::litr::Log::GetClientLogger()->fatal(__VA_ARGS__)

#else

#define LITR_CORE_TRACE(...)
#define LITR_CORE_DEBUG(...)
#define LITR_CORE_INFO(...)
#define LITR_CORE_WARN(...)
#define LITR_CORE_ERROR(...)
#define LITR_CORE_FATAL(...)

#define LITR_TRACE(...)
#define LITR_DEBUG(...)
#define LITR_INFO(...)
#define LITR_WARN(...)
#define LITR_ERROR(...)
#define LITR_FATAL(...)

#endif
