/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "StringUtils.hpp"

#include <algorithm>
#include <codecvt>

#include "Core/Debug/Instrumentor.hpp"

namespace Litr::StringUtils {

std::string trim_left(const std::string& src, char character) {
  LITR_PROFILE_FUNCTION();

  std::string out{src};
  out.erase(0, out.find_first_not_of(character));
  return out;
}

std::string trim_right(const std::string& src, char character) {
  LITR_PROFILE_FUNCTION();

  std::string out{src};
  out.erase(out.find_last_not_of(character) + 1);
  return out;
}

std::string trim(const std::string& src, char character) {
  LITR_PROFILE_FUNCTION();

  return trim_left(trim_right(src, character), character);
}

size_t utf8_length(const std::string& utf8_string) {
  return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}
      .from_bytes(utf8_string)
      .size();
}

void split_into(const std::string& source, const char& delimiter, std::vector<std::string>& out) {
  LITR_PROFILE_FUNCTION();

  std::stringstream ss{source};
  std::string part{};

  while (std::getline(ss, part, delimiter)) {
    out.push_back(part);
  }
}

void split_into(const std::string& source, const char& delimiter, std::deque<std::string>& out) {
  LITR_PROFILE_FUNCTION();

  std::stringstream ss{source};
  std::string part{};

  while (std::getline(ss, part, delimiter)) {
    out.push_back(part);
  }
}

void deduplicate(std::vector<std::string>& items) {
  LITR_PROFILE_FUNCTION();

  std::sort(items.begin(), items.end());
  items.erase(std::unique(items.begin(), items.end()), items.end());
}

std::string replace(const std::string& source, const std::string& from, const std::string& to) {
  LITR_PROFILE_FUNCTION();

  std::string extract{source};
  const size_t start_pos{extract.find(from)};

  if (extract.find(from) != std::string::npos) {
    extract.replace(start_pos, from.length(), to);
  }

  return extract;
}

size_t find_position_by_whole_word(const std::string& value, size_t max_length) {
  LITR_PROFILE_FUNCTION();

  const size_t string_length{StringUtils::utf8_length(value)};
  if (string_length <= max_length) {
    return string_length;
  }

  for (size_t i{max_length}; i > 0; --i) {
    if (value.at(i) == ' ') {
      return i;
    }
  }

  return 0;
}

std::vector<std::string> split_by_whole_word(const std::string& value, size_t max_length) {
  LITR_PROFILE_FUNCTION();

  if (StringUtils::utf8_length(value) <= max_length) {
    return {value};
  }

  std::vector<std::string> lines{};

  const std::function<void(const std::string&)> splitter{[&](const std::string& str) {
    const size_t position{find_position_by_whole_word(str, max_length)};
    const std::string line{trim(str.substr(0, position), ' ')};
    const std::string rest{trim(str.substr(position), ' ')};

    lines.push_back(line);

    if (StringUtils::utf8_length(rest) <= max_length) {
      lines.push_back(rest);
    } else {
      splitter(rest);
    }
  }};

  splitter(value);

  return lines;
}

}  // namespace Litr::StringUtils
