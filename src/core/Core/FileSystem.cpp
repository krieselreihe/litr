/*
* Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
*/

#include "FileSystem.hpp"

#include <utility>

#include "Core/Debug/Instrumentor.hpp"

namespace Litr {

// Path ------------------------------------------

Path::Path(std::string path) : m_Path(std::move(path)) {
}

Path Path::ParentPath() const {
  LITR_PROFILE_FUNCTION();

  return static_cast<Path>(m_Path.parent_path());
}

bool Path::Empty() const {
  LITR_PROFILE_FUNCTION();

  return m_Path.empty();
}

std::string Path::ToString() const {
  LITR_PROFILE_FUNCTION();

  return m_Path.string();
}

Path Path::Append(const std::filesystem::path& path) const {
  return Append(path.string());
}

Path Path::Append(const Path& path) const {
  return Append(path.ToString());
}

Path Path::Append(const std::string& path) const {
  LITR_PROFILE_FUNCTION();

  return static_cast<Path>(m_Path / path);
}

Path Path::WithoutFilename() const {
  auto path{m_Path};
  return Path(path.remove_filename());
}

size_t Path::Count() const {
  size_t count{0};
  for ([[maybe_unused]] auto&& _ : m_Path) {
    count++;
  }
  return count;
}

// FileSystem ------------------------------------

bool FileSystem::Exists(const Path& path) {
  LITR_PROFILE_FUNCTION();

  return std::filesystem::exists(path.m_Path);
}

Path FileSystem::GetCurrentWorkingDirectory() {
  LITR_PROFILE_FUNCTION();

  return static_cast<Path>(std::filesystem::current_path());
}

}  // namespace Litr
