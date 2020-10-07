#pragma once

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

namespace Litr {

class Log {
 private:
  static std::shared_ptr<spdlog::logger> s_Logger;

 public:
  static void Init();

  static std::shared_ptr<spdlog::logger>& GetLogger() {
    return s_Logger;
  }
};

}  // namespace Litr

#define LITR_TRACE(...) ::Litr::Log::GetLogger()->trace(__VA_ARGS__)
#define LITR_INFO(...) ::Litr::Log::GetLogger()->info(__VA_ARGS__)
#define LITR_WARN(...) ::Litr::Log::GetLogger()->warn(__VA_ARGS__)
#define LITR_ERROR(...) ::Litr::Log::GetLogger()->error(__VA_ARGS__)
#define LITR_FATAL(...) ::Litr::Log::GetLogger()->fatal(__VA_ARGS__)
