#include "ConfigFile.hpp"

#include "Core/Environment.hpp"
#include "Core/Debug/Instrumentor.hpp"

namespace Litr {

ConfigFile::ConfigFile(Path cwd) {
  LITR_PROFILE_FUNCTION();

  do {
    LITR_PROFILE_SCOPE("ConfigFile::ConfigFile::ResolvePath(do..while)");

    m_Directory = cwd;
    FindFile(cwd);

    if (m_Status != Status::NOT_FOUND) {
      break;
    }

    cwd = cwd.parent_path();
  } while (cwd != "/");

  Path homeDir{Environment::GetHomeDirectory()};
  if (!homeDir.empty()) {
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

  Path filePath{cwd / m_FileName};
  Path hiddenFilePath{cwd / m_HiddenFileName};

  bool fileExists{std::filesystem::exists(filePath)};
  bool hiddenFileExists{std::filesystem::exists(hiddenFilePath)};

  if (fileExists && hiddenFileExists) {
    m_Status = Status::DUPLICATE;
    return;
  }

  if (fileExists) {
    m_Path = filePath;
    m_Status = Status::FOUND;
    return;
  }

  if (hiddenFileExists) {
    m_Path = hiddenFilePath;
    m_Status = Status::FOUND;
    return;
  }
}

}  // namespace Litr
