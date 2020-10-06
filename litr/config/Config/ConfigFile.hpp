#pragma once

#include <filesystem>
#include <string>

namespace Litr {

using Path = std::filesystem::path;

class ConfigFile {
 public:
  enum class Status {
    FOUND,
    NOT_FOUND,
    DUPLICATE,
  };

  explicit ConfigFile(Path cwd);

  [[nodiscard]] Status GetStatus() const;
  [[nodiscard]] Path GetFilePath() const;
  [[nodiscard]] Path GetFileDirectory() const;

 private:
  std::string m_FileName{"litr.toml"};
  std::string m_HiddenFileName{".litr.toml"};

  Path m_Directory{};
  Path m_Path{};
  Status m_Status{Status::NOT_FOUND};

  void FindFile(const Path& cwd);
  static Path GetHomeDirectory();
};

}  // namespace Litr
