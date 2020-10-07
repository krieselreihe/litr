#include "Log.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Litr {

std::shared_ptr<spdlog::logger> Log::s_Logger;

void Log::Init() {
  std::vector<spdlog::sink_ptr> logSinks{
      std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
      std::make_shared<spdlog::sinks::basic_file_sink_mt>("Litr.log", true)
  };

  logSinks[0]->set_pattern("%^[%T] %n: %v%$");
  logSinks[1]->set_pattern("[%T] [%l] %n: %v");

  s_Logger = std::make_shared<spdlog::logger>("LITR", begin(logSinks), end(logSinks));
  spdlog::register_logger(s_Logger);
  s_Logger->set_level(spdlog::level::trace);
  s_Logger->flush_on(spdlog::level::trace);
}

}  // namespace Litr
