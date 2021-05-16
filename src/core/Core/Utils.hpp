#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <deque>

namespace Litr::Utils {

[[nodiscard]] std::string TrimLeft(const std::string& src, char c);
[[nodiscard]] std::string TrimRight(const std::string& src, char c);
[[nodiscard]] std::string Trim(const std::string& src, char c);

void SplitInto(const std::string& source, const char& delimiter, std::vector<std::string>& out);
void SplitInto(const std::string& source, const char& delimiter, std::deque<std::string>& out);

void Deduplicate(std::vector<std::string>& items);

}  // namespace Litr::Utils
