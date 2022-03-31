/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <deque>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Core/Config/Command.hpp"
#include "Core/Config/Parameter.hpp"
#include "Core/Config/TomlFileAdapter.hpp"
#include "Core/FileSystem.hpp"

namespace Litr::Config {

// @todo: "Loader" is a horrible name. I thought about "File", but this feels rather
//  generic and only works in context of the namespace "config". I have no clue how
//  to name this better thought.
class Loader {
 public:
  using Commands = std::vector<std::shared_ptr<Command>>;
  using Parameters = std::vector<std::shared_ptr<Parameter>>;

  explicit Loader(Path file_path);

  [[nodiscard]] inline Commands get_commands() const {
    return m_commands;
  }
  [[nodiscard]] inline Parameters get_parameters() const {
    return m_parameters;
  }
  [[nodiscard]] inline Path get_file_path() const {
    return m_file_path;
  }

 private:
  std::shared_ptr<Command> create_command(const TomlFileAdapter::Value& commands,
      const TomlFileAdapter::Value& definition,
      const std::string& name);
  void collect_commands(const TomlFileAdapter::Value& commands);
  void collect_params(const TomlFileAdapter::Value& params);

  const Path m_file_path;
  const TomlFileAdapter m_file{};
  Commands m_commands{};
  Parameters m_parameters{};
};

}  // namespace Litr::Config
