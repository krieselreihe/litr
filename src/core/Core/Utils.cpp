#include "Utils.hpp"

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

// @todo: This two functions somehow need to be better, maybe using a template.

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

}  // namespace Litr::Utils
