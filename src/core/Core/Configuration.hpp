#pragma once

#define TOML_EXCEPTIONS 0
#include <tomlplusplus/toml.hpp>

#include "Core/FileSystem.hpp"

namespace Litr {

class Configuration {
 public:
  explicit Configuration(const Path& filePath);
  virtual ~Configuration() = default;

 private:
  toml::table m_RawConfig;
};

}  // namespace Litr
