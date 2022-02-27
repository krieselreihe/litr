/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "FileResolver.hpp"

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Environment.hpp"
#include "Core/Log.hpp"

namespace litr::config {

FileResolver::FileResolver(const std::string& cwd) : FileResolver(Path(cwd)) {
}

FileResolver::FileResolver(Path cwd) {
  LITR_PROFILE_FUNCTION();

  do {
    LITR_PROFILE_SCOPE("Config::FileResolver::ResolvePath(do..while)");

    m_directory = cwd;
    LITR_CORE_TRACE("Searching configuration file in: {0}", m_directory);
    find_file(cwd);

    if (m_status != Status::NOT_FOUND) {
      return;
    }

    cwd = cwd.parent_path();
  } while (cwd != cwd.parent_path());

  Path home_dir{Environment::get_home_directory()};
  if (!home_dir.empty()) {
    LITR_CORE_TRACE("Searching configuration file in user home: {0}", home_dir);
    find_file(home_dir);
  }
}

FileResolver::Status FileResolver::get_status() const {
  return m_status;
}

Path FileResolver::get_file_path() const {
  return m_path;
}

Path FileResolver::get_file_directory() const {
  return m_directory;
}

void FileResolver::find_file(const Path& cwd) {
  LITR_PROFILE_FUNCTION();

  const Path file_path{cwd.append(m_file_name)};
  const Path hidden_file_path{cwd.append(m_hidden_filename)};

  bool file_exists{FileSystem::exists(file_path)};
  bool hidden_file_exists{FileSystem::exists(hidden_file_path)};

  if (file_exists && hidden_file_exists) {
    LITR_CORE_TRACE("Configuration file duplicate detected.");
    m_status = Status::DUPLICATE;
    return;
  }

  if (file_exists) {
    LITR_CORE_TRACE("Configuration file found at: {0}", file_path);
    m_path = file_path;
    m_status = Status::FOUND;
    return;
  }

  if (hidden_file_exists) {
    LITR_CORE_TRACE("Hidden configuration file found at: {0}", hidden_file_path);
    m_path = hidden_file_path;
    m_status = Status::FOUND;
    return;
  }
}

}  // namespace litr::config
