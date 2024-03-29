/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>

#include "Core/FileSystem.hpp"

namespace Litr::Config {

class FileResolver {
 public:
  enum class Status {
    FOUND,
    NOT_FOUND,
    DUPLICATE,
  };

  explicit FileResolver(const std::string& cwd);
  explicit FileResolver(Path cwd);

  [[nodiscard]] Status get_status() const;
  [[nodiscard]] Path get_file_path() const;
  [[nodiscard]] Path get_file_directory() const;

 private:
  const std::string m_file_name{"litr.toml"};
  const std::string m_hidden_filename{".litr.toml"};

  Path m_directory{};
  Path m_path{};
  Status m_status{Status::NOT_FOUND};

  void find_file(const Path& cwd);
};

}  // namespace Litr::Config
