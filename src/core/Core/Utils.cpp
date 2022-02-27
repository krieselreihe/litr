/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Utils.hpp"

#include <algorithm>

#include "Core/Debug/Instrumentor.hpp"

namespace Litr::Utils {

std::string TrimLeft(const std::string& src, char c) {
  LITR_PROFILE_FUNCTION();

  std::string out{src};
  out.erase(0, out.find_first_not_of(c));
  return out;
}

std::string TrimRight(const std::string& src, char c) {
  LITR_PROFILE_FUNCTION();

  std::string out{src};
  out.erase(out.find_last_not_of(c) + 1);
  return out;
}

std::string Trim(const std::string& src, char c) {
  LITR_PROFILE_FUNCTION();

  return TrimLeft(TrimRight(src, c), c);
}

void SplitInto(const std::string& source, const char& delimiter, std::vector<std::string>& out) {
  LITR_PROFILE_FUNCTION();

  std::stringstream ss{source};
  std::string part{};

  while (std::getline(ss, part, delimiter)) {
    out.push_back(part);
  }
}

void SplitInto(const std::string& source, const char& delimiter, std::deque<std::string>& out) {
  LITR_PROFILE_FUNCTION();

  std::stringstream ss{source};
  std::string part{};

  while (std::getline(ss, part, delimiter)) {
    out.push_back(part);
  }
}

void Deduplicate(std::vector<std::string>& items) {
  LITR_PROFILE_FUNCTION();

  std::sort(items.begin(), items.end());
  items.erase(std::unique(items.begin(), items.end()), items.end());
}

std::string Replace(const std::string& source, const std::string& from, const std::string& to) {
  LITR_PROFILE_FUNCTION();

  std::string extract{source};
  const size_t startPos{extract.find(from)};

  if (extract.find(from) != std::string::npos) {
    extract.replace(startPos, from.length(), to);
  }

  return extract;
}

}  // namespace Litr::Utils
