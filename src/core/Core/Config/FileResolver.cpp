#include "FileResolver.hpp"

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Environment.hpp"
#include "Core/Log.hpp"

namespace Litr::Config {

FileResolver::FileResolver(const std::string& cwd) : FileResolver(Path(cwd)) {
}

FileResolver::FileResolver(Path cwd) {
  LITR_PROFILE_FUNCTION();

  do {
    LITR_PROFILE_SCOPE("Config::FileResolver::ResolvePath(do..while)");

    m_Directory = cwd;
    LITR_CORE_TRACE("Searching configuration file in: {0}", m_Directory);
    FindFile(cwd);

    if (m_Status != Status::NOT_FOUND) {
      return;
    }

    cwd = cwd.ParentPath();
  } while (cwd != cwd.ParentPath());

  Path homeDir{Environment::GetHomeDirectory()};
  if (!homeDir.Empty()) {
    LITR_CORE_TRACE("Searching configuration file in user home: {0}", homeDir);
    FindFile(homeDir);
  }
}

FileResolver::Status FileResolver::GetStatus() const {
  return m_Status;
}

Path FileResolver::GetFilePath() const {
  return m_Path;
}

Path FileResolver::GetFileDirectory() const {
  return m_Directory;
}

void FileResolver::FindFile(const Path& cwd) {
  LITR_PROFILE_FUNCTION();

  const Path filePath{cwd.Append(m_FileName)};
  const Path hiddenFilePath{cwd.Append(m_HiddenFileName)};

  bool fileExists{FileSystem::Exists(filePath)};
  bool hiddenFileExists{FileSystem::Exists(hiddenFilePath)};

  if (fileExists && hiddenFileExists) {
    LITR_CORE_TRACE("Configuration file duplicate detected.");
    m_Status = Status::DUPLICATE;
    return;
  }

  if (fileExists) {
    LITR_CORE_TRACE("Configuration file found at: {0}", filePath);
    m_Path = filePath;
    m_Status = Status::FOUND;
    return;
  }

  if (hiddenFileExists) {
    LITR_CORE_TRACE("Hidden configuration file found at: {0}", hiddenFilePath);
    m_Path = hiddenFilePath;
    m_Status = Status::FOUND;
    return;
  }
}

}  // namespace Litr::Config
