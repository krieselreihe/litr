#pragma once

#include <string>

#include "Core/FileSystem.hpp"

namespace Litr {

class ConfigFileResolver {
 public:
  enum class Status {
    FOUND,
    NOT_FOUND,
    DUPLICATE,
  };

  explicit ConfigFileResolver(Path cwd);
  explicit ConfigFileResolver(const std::string& cwd);

  [[nodiscard]] Status GetStatus() const;
  [[nodiscard]] Path GetFilePath() const;
  [[nodiscard]] Path GetFileDirectory() const;

 private:
  const std::string m_FileName{"litr.toml"};
  const std::string m_HiddenFileName{".litr.toml"};

  Path m_Directory{};
  Path m_Path{};
  Status m_Status{Status::NOT_FOUND};

  void FindFile(const Path& cwd);
};

}  // namespace Litr
