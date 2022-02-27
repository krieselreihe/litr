/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

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

std::string Replace(const std::string& source, const std::string& from, const std::string& to);

}  // namespace Litr::Utils
