/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <fmt/format.h>
#include <spdlog/fmt/ostr.h>

#include <filesystem>
#include <string>

namespace Litr {

class Path {
 public:
  using Iterator = std::filesystem::path::iterator;
  using ConstIterator = std::filesystem::path::const_iterator;

  Path() = default;
  explicit Path(std::string path);

  [[nodiscard]] Path parent_path() const;
  [[nodiscard]] bool empty() const;
  [[nodiscard]] std::string to_string() const;

  [[nodiscard]] Path append(const std::filesystem::path& path) const;
  [[nodiscard]] Path append(const std::string& path) const;
  [[nodiscard]] Path append(const Path& path) const;

  [[nodiscard]] Path without_filename() const;

  [[nodiscard]] size_t count() const;

  explicit operator std::string() const {
    return to_string();
  }

  bool operator!=(const Path& other) const {
    return other.m_path != m_path;
  }
  bool operator==(const Path& other) const {
    return other.m_path == m_path;
  }
  bool operator!=(const std::string& other) const {
    return other != m_path;
  }
  bool operator==(const std::string& other) const {
    return other == m_path;
  }

  Iterator begin() {
    return m_path.begin();
  }
  Iterator end() {
    return m_path.end();
  }
  [[nodiscard]] ConstIterator begin() const {
    return m_path.begin();
  }
  [[nodiscard]] ConstIterator end() const {
    return m_path.end();
  }

  template <typename OStream>
  friend OStream& operator<<(OStream& ostream, const Path& path) {
    return ostream << path.to_string();
  }

 private:
  std::filesystem::path m_path{};
  friend class FileSystem;
};

class FileSystem {
 public:
  [[nodiscard]] static bool exists(const Path& path);
  [[nodiscard]] static Path get_current_working_directory();
};

}  // namespace Litr

// Enable easy formatting with fmt
template <>
struct fmt::formatter<Litr::Path> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Litr::Path& path, FormatContext& ctx) {
    return format_to(ctx.out(), "{}", path.to_string());
  }
};
