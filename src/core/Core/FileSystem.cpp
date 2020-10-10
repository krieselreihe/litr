#include "FileSystem.hpp"

namespace Litr {

// Path ------------------------------------------

Path::Path(std::string path) : m_Path(std::move(path)) {
}

Path::Path(std::filesystem::path path) : m_Path(std::move(path)) {
}

Path Path::ParentPath() const {
  return static_cast<Path>(m_Path.parent_path());
}

bool Path::Empty() const {
  return m_Path.empty();
}

std::string Path::ToString() const {
  return m_Path.string();
}

Path Path::Append(const std::filesystem::path& path) const {
  return Append(path.string());
}

Path Path::Append(const Path& path) const {
  return Append(path.ToString());
}

Path Path::Append(const std::string& path) const {
  return static_cast<Path>(m_Path / path);
}

// FileSystem ------------------------------------

bool FileSystem::Exists(const Path& path) {
  return std::filesystem::exists(path.m_Path);
}

Path FileSystem::GetCurrentWorkingDirectory() {
  return static_cast<Path>(std::filesystem::current_path());
}

}  // namespace Litr
