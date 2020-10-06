#include "ConfigFile.hpp"

#include <cstdlib>

#include <fmt/format.h>

namespace Litr {

ConfigFile::ConfigFile(Path cwd) {
  do {
    m_Directory = cwd;
    FindFile(cwd);

    if (m_Status != Status::NOT_FOUND) {
      break;
    }

    cwd = cwd.parent_path();
  } while (cwd != "/");

  Path homeDir{GetHomeDirectory()};
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

Path ConfigFile::GetHomeDirectory() {
#ifdef LITR_PLATFORM_MACOS
  std::string userName{std::getenv("USER")};
  std::string loginName{std::getenv("LOGNAME")};
  Path path{fmt::format("/Users/{}", userName.empty() ? userName : loginName)};
#elif LITR_PLATFORM_WINDOWS
  Path path{std::getenv("HOMEPATH")};
#elif LITR_PLATFORM_LINUX
  Path path{std::getenv("HOME")};
#endif

  return path;
}

}  // namespace Litr
