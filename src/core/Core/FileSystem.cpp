/*
* Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
*/

#include "FileSystem.hpp"

#include <utility>

#include "Core/Debug/Instrumentor.hpp"

namespace litr {

// Path ------------------------------------------

Path::Path(std::string path) : m_path(std::move(path)) {
}

Path Path::parent_path() const {
  LITR_PROFILE_FUNCTION();

  return static_cast<Path>(m_path.parent_path());
}

bool Path::empty() const {
  LITR_PROFILE_FUNCTION();

  return m_path.empty();
}

std::string Path::to_string() const {
  LITR_PROFILE_FUNCTION();

  return m_path.string();
}

Path Path::append(const std::filesystem::path& path) const {
  return append(path.string());
}

Path Path::append(const Path& path) const {
  return append(path.to_string());
}

Path Path::append(const std::string& path) const {
  LITR_PROFILE_FUNCTION();

  return static_cast<Path>(m_path / path);
}

Path Path::without_filename() const {
  auto path{m_path};
  return Path(path.remove_filename());
}

size_t Path::count() const {
  size_t count{0};
  for ([[maybe_unused]] auto&& _ : m_path) {
    count++;
  }
  return count;
}

// FileSystem ------------------------------------

bool FileSystem::exists(const Path& path) {
  LITR_PROFILE_FUNCTION();

  return std::filesystem::exists(path.m_path);
}

Path FileSystem::get_current_working_directory() {
  LITR_PROFILE_FUNCTION();

  return Path(std::filesystem::current_path());
}

}  // namespace litr
