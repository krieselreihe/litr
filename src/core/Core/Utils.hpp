/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <deque>
#include <sstream>
#include <string>
#include <vector>

namespace litr::utils {

[[nodiscard]] std::string trim_left(const std::string& src, char character);
[[nodiscard]] std::string trim_right(const std::string& src, char character);
[[nodiscard]] std::string trim(const std::string& src, char character);

void split_into(const std::string& source, const char& delimiter, std::vector<std::string>& out);
void split_into(const std::string& source, const char& delimiter, std::deque<std::string>& out);

void deduplicate(std::vector<std::string>& items);

std::string replace(const std::string& source, const std::string& from, const std::string& to);

}  // namespace litr::utils
