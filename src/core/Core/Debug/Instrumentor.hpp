/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>

#include "Core/Log.hpp"

namespace litr::debug {

using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

struct ProfileResult {
  std::string name;
  FloatingPointMicroseconds start;
  std::chrono::microseconds elapsed_time;
  std::thread::id thread_id;
};

struct InstrumentationSession {
  const std::string name;
};

class Instrumentor {
 public:
  Instrumentor(const Instrumentor&) = delete;
  Instrumentor(Instrumentor&&) = delete;

  void begin_session(const std::string& name, const std::string& filepath = "results.json") {
    std::lock_guard lock(m_mutex);

    if (m_current_session) {
      // If there is already a current session, then close it before beginning new one.
      // Subsequent profiling output meant for the original session will end up in the
      // newly opened session instead.  That's better than having badly formatted
      // profiling output.
      LITR_CORE_ERROR("Instrumentor::begin_session('{0}') when session '{1}' already open.", name, m_current_session->name);
      internal_end_session();
    }
    m_output_stream.open(filepath);

    if (m_output_stream.is_open()) {
      m_current_session = new InstrumentationSession({name});
      write_header();
    } else {
      LITR_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
    }
  }

  void end_session() {
    std::lock_guard lock(m_mutex);
    internal_end_session();
  }

  void write_profile(const ProfileResult& result) {
    std::stringstream json;

    std::string name{result.name};
    std::replace(name.begin(), name.end(), '"', '\'');

    json << std::setprecision(3) << std::fixed;
    json << ",{";
    json << R"("cat":"function",)";
    json << "\"dur\":" << (result.elapsed_time.count()) << ',';
    json << R"("name":")" << name << "\",";
    json << R"("ph":"X",)";
    json << "\"pid\":0,";
    json << R"("tid":")" << result.thread_id << "\",";
    json << "\"ts\":" << result.start.count();
    json << "}";

    std::lock_guard lock(m_mutex);
    if (m_current_session) {
      m_output_stream << json.str();
      m_output_stream.flush();
    }
  }

  static Instrumentor& get() {
    static Instrumentor instance;
    return instance;
  }

 private:
  Instrumentor() : m_current_session(nullptr) {
  }

  ~Instrumentor() {
    end_session();
  }

  void write_header() {
    m_output_stream << R"({"otherData": {},"traceEvents":[{})";
    m_output_stream.flush();
  }

  void write_footer() {
    m_output_stream << "]}";
    m_output_stream.flush();
  }

  // Note: you must already own lock on m_Mutex before
  // calling InternalEndSession()
  void internal_end_session() {
    if (m_current_session) {
      write_footer();
      m_output_stream.close();
      delete m_current_session;
      m_current_session = nullptr;
    }
  }

  std::mutex m_mutex;
  InstrumentationSession* m_current_session;
  std::ofstream m_output_stream;
};

class InstrumentationTimer {
 public:
  explicit InstrumentationTimer(const char* name)
      : m_name(name), m_stopped(false), m_start_time_point(std::chrono::steady_clock::now()) {
  }

  ~InstrumentationTimer() {
    if (!m_stopped) {
      stop();
    }
  }

  void stop() {
    const auto end_time_point{std::chrono::steady_clock::now()};
    const auto high_res_start{FloatingPointMicroseconds{m_start_time_point.time_since_epoch()}};
    const auto elapsed_time{
      std::chrono::time_point_cast<std::chrono::microseconds>(end_time_point).time_since_epoch() -
        std::chrono::time_point_cast<std::chrono::microseconds>(m_start_time_point).time_since_epoch()
    };

    Instrumentor::get().write_profile({m_name, high_res_start, elapsed_time, std::this_thread::get_id()});

    m_stopped = true;
  }

 private:
  const char* m_name;
  bool m_stopped{false};
  const std::chrono::time_point<std::chrono::steady_clock> m_start_time_point;
};

}  // namespace litr::debug

#if LITR_PROFILE
// Resolve which function signature macro will be used. Note that this only
// is resolved when the (pre)compiler starts, so the syntax highlighting
// could mark the wrong one in your editor!
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || \
    defined(__ghs__)
#define LITR_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define LITR_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
#define LITR_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define LITR_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define LITR_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define LITR_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define LITR_FUNC_SIG __func__
#else
#define LITR_FUNC_SIG "LITR_FUNC_SIG unknown!"
#endif

#define JOIN_AGAIN(x, y) x##y
#define JOIN(x, y) JOIN_AGAIN(x, y)
#define LITR_PROFILE_BEGIN_SESSION(name) ::litr::debug::Instrumentor::get().begin_session(name)
#define LITR_PROFILE_BEGIN_SESSION_WITH_FILE(name, file_path) \
  ::litr::debug::Instrumentor::get().begin_session(name, file_path)
#define LITR_PROFILE_END_SESSION() ::litr::debug::Instrumentor::get().end_session()
#define LITR_PROFILE_SCOPE(name)                              \
  ::litr::debug::InstrumentationTimer JOIN(timer, __LINE__) { \
    name                                                      \
  }
#define LITR_PROFILE_FUNCTION() LITR_PROFILE_SCOPE(LITR_FUNC_SIG)
#else
#define LITR_PROFILE_BEGIN_SESSION(name)
#define LITR_PROFILE_BEGIN_SESSION_WITH_FILE(name, file_path)
#define LITR_PROFILE_END_SESSION()
#define LITR_PROFILE_SCOPE(name)
#define LITR_PROFILE_FUNCTION()
#endif
