#pragma once

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#include "Core/Base.hpp"

namespace Litr {

class Log {
 private:
  static Ref<spdlog::logger> s_CoreLogger;
  static Ref<spdlog::logger> s_ClientLogger;

 public:
  static void Init();

  static Ref<spdlog::logger>& GetCoreLogger() {
    return s_CoreLogger;
  }

  static Ref<spdlog::logger>& GetClientLogger() {
    return s_ClientLogger;
  }
};

}  // namespace Litr

// Core logging

#ifndef LITR_DEACTIVATE_LOGGING

#if DEBUG
#define LITR_CORE_TRACE(...) ::Litr::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LITR_CORE_DEBUG(...) ::Litr::Log::GetCoreLogger()->debug(__VA_ARGS__)
#else
#define LITR_CORE_TRACE(...)
#define LITR_CORE_DEBUG(...)
#endif

#define LITR_CORE_INFO(...) ::Litr::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LITR_CORE_WARN(...) ::Litr::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LITR_CORE_ERROR(...) ::Litr::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LITR_CORE_FATAL(...) ::Litr::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client logging

#if DEBUG
#define LITR_TRACE(...) ::Litr::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LITR_DEBUG(...) ::Litr::Log::GetClientLogger()->debug(__VA_ARGS__)
#else
#define LITR_TRACE(...)
#define LITR_DEBUG(...)
#endif

#define LITR_INFO(...) ::Litr::Log::GetClientLogger()->info(__VA_ARGS__)
#define LITR_WARN(...) ::Litr::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LITR_ERROR(...) ::Litr::Log::GetClientLogger()->error(__VA_ARGS__)
#define LITR_FATAL(...) ::Litr::Log::GetClientLogger()->fatal(__VA_ARGS__)

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
