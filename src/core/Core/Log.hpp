/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#include <memory>

namespace Litr {

class Log {
 public:
  Log(const Log&) = delete;
  Log(const Log&&) = delete;
  Log& operator=(const Log&) = delete;
  Log& operator=(const Log&&) = delete;
  ~Log() = default;

  static std::shared_ptr<spdlog::logger>& get_core_logger() {
    return get().m_core_logger;
  }

  static std::shared_ptr<spdlog::logger>& get_client_logger() {
    return get().m_client_logger;
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

  std::shared_ptr<spdlog::logger> m_core_logger;
  std::shared_ptr<spdlog::logger> m_client_logger;
};

}  // namespace Litr

// Core logging

#ifndef LITR_DEACTIVATE_LOGGING

#if DEBUG
#define LITR_CORE_TRACE(...) ::Litr::Log::get_core_logger()->trace(__VA_ARGS__)
#define LITR_CORE_DEBUG(...) ::Litr::Log::get_core_logger()->debug(__VA_ARGS__)
#else
#define LITR_CORE_TRACE(...)
#define LITR_CORE_DEBUG(...)
#endif

#define LITR_CORE_INFO(...) ::Litr::Log::get_core_logger()->info(__VA_ARGS__)
#define LITR_CORE_WARN(...) ::Litr::Log::get_core_logger()->warn(__VA_ARGS__)
#define LITR_CORE_ERROR(...) ::Litr::Log::get_core_logger()->error(__VA_ARGS__)
#define LITR_CORE_FATAL(...) ::Litr::Log::get_core_logger()->fatal(__VA_ARGS__)

// Client logging

#if DEBUG
#define LITR_TRACE(...) ::Litr::Log::get_client_logger()->trace(__VA_ARGS__)
#define LITR_DEBUG(...) ::Litr::Log::get_client_logger()->debug(__VA_ARGS__)
#else
#define LITR_TRACE(...)
#define LITR_DEBUG(...)
#endif

#define LITR_INFO(...) ::Litr::Log::get_client_logger()->info(__VA_ARGS__)
#define LITR_WARN(...) ::Litr::Log::get_client_logger()->warn(__VA_ARGS__)
#define LITR_ERROR(...) ::Litr::Log::get_client_logger()->error(__VA_ARGS__)
#define LITR_FATAL(...) ::Litr::Log::get_client_logger()->fatal(__VA_ARGS__)

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
