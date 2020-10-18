#include "Log.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Litr {

Ref<spdlog::logger> Log::s_CoreLogger;
Ref<spdlog::logger> Log::s_ClientLogger;

void Log::Init() {
#ifndef LITR_DEACTIVATE_LOGGING
  std::vector<spdlog::sink_ptr> logSinks;

#ifdef TRACE
  spdlog::level::level_enum level{spdlog::level::trace};
#else
  spdlog::level::level_enum level{spdlog::level::debug};
#endif

  logSinks.emplace_back(CreateRef<spdlog::sinks::stdout_color_sink_mt>());
  logSinks.emplace_back(CreateRef<spdlog::sinks::basic_file_sink_mt>("Litr.log", true));

  logSinks[0]->set_pattern("%^[%T] %n(%l): %v%$");
  logSinks[1]->set_pattern("[%T] [%l] %n(%l): %v");

  s_CoreLogger = CreateRef<spdlog::logger>("CORE", begin(logSinks), end(logSinks));
  spdlog::register_logger(s_CoreLogger);
  s_CoreLogger->set_level(level);
  s_CoreLogger->flush_on(level);

  s_ClientLogger = CreateRef<spdlog::logger>("CLIENT", begin(logSinks), end(logSinks));
  spdlog::register_logger(s_ClientLogger);
  s_ClientLogger->set_level(level);
  s_ClientLogger->flush_on(level);
#endif
}

}  // namespace Litr
