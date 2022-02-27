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

namespace litr::Debug {

using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

struct ProfileResult {
  std::string Name;
  FloatingPointMicroseconds Start;
  std::chrono::microseconds ElapsedTime;
  std::thread::id ThreadID;
};

struct InstrumentationSession {
  const std::string Name;
};

class Instrumentor {
 public:
  Instrumentor(const Instrumentor&) = delete;
  Instrumentor(Instrumentor&&) = delete;

  void BeginSession(const std::string& name, const std::string& filepath = "results.json") {
    std::lock_guard lock(m_Mutex);

    if (m_CurrentSession) {
      // If there is already a current session, then close it before beginning new one.
      // Subsequent profiling output meant for the original session will end up in the
      // newly opened session instead.  That's better than having badly formatted
      // profiling output.
      LITR_CORE_ERROR("Instrumentor::BeginSession('{0}') when session '{1}' already open.", name, m_CurrentSession->Name);
      InternalEndSession();
    }
    m_OutputStream.open(filepath);

    if (m_OutputStream.is_open()) {
      m_CurrentSession = new InstrumentationSession({name});
      WriteHeader();
    } else {
      LITR_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
    }
  }

  void EndSession() {
    std::lock_guard lock(m_Mutex);
    InternalEndSession();
  }

  void WriteProfile(const ProfileResult& result) {
    std::stringstream json;

    std::string name{result.Name};
    std::replace(name.begin(), name.end(), '"', '\'');

    json << std::setprecision(3) << std::fixed;
    json << ",{";
    json << R"("cat":"function",)";
    json << "\"dur\":" << (result.ElapsedTime.count()) << ',';
    json << R"("name":")" << name << "\",";
    json << R"("ph":"X",)";
    json << "\"pid\":0,";
    json << R"("tid":")" << result.ThreadID << "\",";
    json << "\"ts\":" << result.Start.count();
    json << "}";

    std::lock_guard lock(m_Mutex);
    if (m_CurrentSession) {
      m_OutputStream << json.str();
      m_OutputStream.flush();
    }
  }

  static Instrumentor& Get() {
    static Instrumentor instance;
    return instance;
  }

 private:
  Instrumentor() : m_CurrentSession(nullptr) {
  }

  ~Instrumentor() {
    EndSession();
  }

  void WriteHeader() {
    m_OutputStream << R"({"otherData": {},"traceEvents":[{})";
    m_OutputStream.flush();
  }

  void WriteFooter() {
    m_OutputStream << "]}";
    m_OutputStream.flush();
  }

  // Note: you must already own lock on m_Mutex before
  // calling InternalEndSession()
  void InternalEndSession() {
    if (m_CurrentSession) {
      WriteFooter();
      m_OutputStream.close();
      delete m_CurrentSession;
      m_CurrentSession = nullptr;
    }
  }

 private:
  std::mutex m_Mutex;
  InstrumentationSession* m_CurrentSession;
  std::ofstream m_OutputStream;
};

class InstrumentationTimer {
 public:
  explicit InstrumentationTimer(const char* name)
      : m_Name(name), m_Stopped(false), m_StartTimePoint(std::chrono::steady_clock::now()) {
  }

  ~InstrumentationTimer() {
    if (!m_Stopped) {
      Stop();
    }
  }

  void Stop() {
    const auto endTimePoint{std::chrono::steady_clock::now()};
    const auto highResStart{FloatingPointMicroseconds{m_StartTimePoint.time_since_epoch()}};
    const auto elapsedTime{
      std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch() -
        std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimePoint).time_since_epoch()
    };

    Instrumentor::Get().WriteProfile({m_Name, highResStart, elapsedTime, std::this_thread::get_id()});

    m_Stopped = true;
  }

 private:
  const char* m_Name;
  bool m_Stopped{false};
  const std::chrono::time_point<std::chrono::steady_clock> m_StartTimePoint;
};

}  // namespace litr::Debug

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
#define LITR_PROFILE_BEGIN_SESSION(name) ::litr::Debug::Instrumentor::Get().BeginSession(name)
#define LITR_PROFILE_BEGIN_SESSION_WITH_FILE(name, filePath) \
  ::litr::Debug::Instrumentor::Get().BeginSession(name, filePath)
#define LITR_PROFILE_END_SESSION() ::litr::Debug::Instrumentor::Get().EndSession()
#define LITR_PROFILE_SCOPE(name)                              \
  ::litr::Debug::InstrumentationTimer JOIN(timer, __LINE__) { \
    name                                                      \
  }
#define LITR_PROFILE_FUNCTION() LITR_PROFILE_SCOPE(LITR_FUNC_SIG)
#else
#define LITR_PROFILE_BEGIN_SESSION(name)
#define LITR_PROFILE_BEGIN_SESSION_WITH_FILE(name, filePath)
#define LITR_PROFILE_END_SESSION()
#define LITR_PROFILE_SCOPE(name)
#define LITR_PROFILE_FUNCTION()
#endif
