/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <deque>
#include <utility>
#include <vector>
#include <string>

#include <toml.hpp>

#include "Core/Base.hpp"
#include "Core/Config/Command.hpp"
#include "Core/Config/Parameter.hpp"
#include "Core/FileSystem.hpp"

namespace litr::Config {

class Loader {
 public:
  using Commands = std::vector<Ref<Command>>;
  using Parameters = std::vector<Ref<Parameter>>;

  explicit Loader(const Path& filePath);
  ~Loader() = default;

  [[nodiscard]] inline Commands GetCommands() const { return m_Commands; }
  [[nodiscard]] inline Parameters GetParameters() const { return m_Parameters; }
  [[nodiscard]] inline Path GetFilePath() const { return m_FilePath; }

 private:
  Ref<Command> CreateCommand(const toml::table& commands, const toml::value& definition, const std::string& name);
  void CollectCommands(const toml::table& commands);
  void CollectParams(const toml::table& params);

 private:
  const Path m_FilePath;

  Commands m_Commands{};
  Parameters m_Parameters{};
};

}  // namespace litr::Config
