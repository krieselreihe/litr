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

  static Ref<spdlog::logger>& get_core_logger() {
    return get().s_core_logger;
  }

  static Ref<spdlog::logger>& get_client_logger() {
    return get().s_client_logger;
  }

 private:
  // The constructor shall not be deleted but used to bootstrap the logger. Ignoring
  // the lint warning is ignoring doing `Log() = delete`.
  // NOLINTNEXTLINE
  Log();

  static Log& get() {
    static Log instance{};
    return instance;
  }

  Ref<spdlog::logger> s_core_logger;
  Ref<spdlog::logger> s_client_logger;
};

}  // namespace litr

// Core logging

#ifndef LITR_DEACTIVATE_LOGGING

#if DEBUG
#define LITR_CORE_TRACE(...) ::litr::Log::get_core_logger()->trace(__VA_ARGS__)
#define LITR_CORE_DEBUG(...) ::litr::Log::get_core_logger()->debug(__VA_ARGS__)
#else
#define LITR_CORE_TRACE(...)
#define LITR_CORE_DEBUG(...)
#endif

#define LITR_CORE_INFO(...) ::litr::Log::get_core_logger()->info(__VA_ARGS__)
#define LITR_CORE_WARN(...) ::litr::Log::get_core_logger()->warn(__VA_ARGS__)
#define LITR_CORE_ERROR(...) ::litr::Log::get_core_logger()->error(__VA_ARGS__)
#define LITR_CORE_FATAL(...) ::litr::Log::get_core_logger()->fatal(__VA_ARGS__)

// Client logging

#if DEBUG
#define LITR_TRACE(...) ::litr::Log::get_client_logger()->trace(__VA_ARGS__)
#define LITR_DEBUG(...) ::litr::Log::get_client_logger()->debug(__VA_ARGS__)
#else
#define LITR_TRACE(...)
#define LITR_DEBUG(...)
#endif

#define LITR_INFO(...) ::litr::Log::get_client_logger()->info(__VA_ARGS__)
#define LITR_WARN(...) ::litr::Log::get_client_logger()->warn(__VA_ARGS__)
#define LITR_ERROR(...) ::litr::Log::get_client_logger()->error(__VA_ARGS__)
#define LITR_FATAL(...) ::litr::Log::get_client_logger()->fatal(__VA_ARGS__)

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
