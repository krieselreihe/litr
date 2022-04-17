/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Log.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <vector>

namespace Litr {

Log::Log() {
  std::vector<spdlog::sink_ptr> log_sinks;

#ifdef TRACE
  const spdlog::level::level_enum level{spdlog::level::trace};
#else
  const spdlog::level::level_enum level{spdlog::level::debug};
#endif

  log_sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
  log_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("litr.log", true));

  log_sinks[0]->set_pattern("%^[%T] %n(%l): %v%$");
  log_sinks[1]->set_pattern("[%T] [%l] %n(%l): %v");

  m_core_logger = std::make_shared<spdlog::logger>("CORE", begin(log_sinks), end(log_sinks));
  spdlog::register_logger(m_core_logger);
  m_core_logger->set_level(level);
  m_core_logger->flush_on(level);

  m_client_logger = std::make_shared<spdlog::logger>("CLIENT", begin(log_sinks), end(log_sinks));
  spdlog::register_logger(m_client_logger);
  spdlog::set_default_logger(m_client_logger);
  m_client_logger->set_level(level);
  m_client_logger->flush_on(level);
}

}  // namespace Litr
