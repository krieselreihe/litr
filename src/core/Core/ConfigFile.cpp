#include "ConfigFile.hpp"

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Environment.hpp"
#include "Core/Log.hpp"

namespace Litr {

ConfigFile::ConfigFile(const std::string& cwd) : ConfigFile(static_cast<Path>(cwd)) {
}

ConfigFile::ConfigFile(Path cwd) {
  LITR_PROFILE_FUNCTION();

  do {
    LITR_PROFILE_SCOPE("ConfigFile::ConfigFile::ResolvePath(do..while)");

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

ConfigFile::Status ConfigFile::GetStatus() const {
  return m_Status;
}

Path ConfigFile::GetFilePath() const {
  return m_Path;
}

Path ConfigFile::GetFileDirectory() const {
  return m_Directory;
}

void ConfigFile::FindFile(const Path& cwd) {
  LITR_PROFILE_FUNCTION();

  Path filePath{cwd.Append(m_FileName)};
  Path hiddenFilePath{cwd.Append(m_HiddenFileName)};

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

}  // namespace Litr
