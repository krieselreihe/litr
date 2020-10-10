#include "FileSystem.hpp"

#include "Core/Debug/Instrumentor.hpp"

namespace Litr {

// Path ------------------------------------------

Path::Path(std::string path) : m_Path(std::move(path)) {
}

Path::Path(std::filesystem::path path) : m_Path(std::move(path)) {
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
