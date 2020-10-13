#pragma once

#include <vector>

#define TOML_EXCEPTIONS 0
#include <tomlplusplus/toml.hpp>

#include "Core/FileSystem.hpp"
#include "Core/Command.hpp"

namespace Litr {

class Configuration {
 public:
  explicit Configuration(const Path& filePath);
  virtual ~Configuration() = default;

 private:
  void CollectCommands();
  void CollectParams();

 private:
  toml::table m_RawConfig;
  std::vector<Command> m_Commands{};
};

}  // namespace Litr
