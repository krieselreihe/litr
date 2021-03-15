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
  virtual ~Path() = default;

  [[nodiscard]] Path ParentPath() const;
  [[nodiscard]] bool Empty() const;
  [[nodiscard]] std::string ToString() const;

  [[nodiscard]] Path Append(const std::filesystem::path& path) const;
  [[nodiscard]] Path Append(const std::string& path) const;
  [[nodiscard]] Path Append(const Path& path) const;

  [[nodiscard]] size_t Count() const;

  explicit operator std::string() const { return ToString(); }

  bool operator!=(const Path& other) const { return other.m_Path != m_Path; }
  bool operator==(const Path& other) const { return other.m_Path == m_Path; }
  bool operator!=(const std::string& other) const { return other != m_Path; }
  bool operator==(const std::string& other) const { return other == m_Path; }

  Iterator begin() { return m_Path.begin(); }
  Iterator end() { return m_Path.end(); }
  ConstIterator begin() const { return m_Path.begin(); }
  ConstIterator end() const { return m_Path.end(); }

  template <typename OStream>
  friend OStream& operator<<(OStream& os, const Path& path) {
    return os << path.ToString();
  }

 private:
  std::filesystem::path m_Path{};
  friend class FileSystem;
};

class FileSystem {
 public:
  [[nodiscard]] static bool Exists(const Path& path);
  [[nodiscard]] static Path GetCurrentWorkingDirectory();
};

}  // namespace Litr

#ifdef DEBUG
// Enable easy formatting with fmt
template <>
struct fmt::formatter<Litr::Path> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Litr::Path& p, FormatContext& ctx) {
    return format_to(ctx.out(), "{}", p.ToString());
  }
};
#endif
