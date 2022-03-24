/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <deque>
#include <memory>
#include <string>
#include <toml.hpp>
#include <utility>
#include <vector>

#include "Core/Config/Command.hpp"
#include "Core/Config/Parameter.hpp"
#include "Core/FileSystem.hpp"

namespace litr::config {

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
  std::shared_ptr<Command> create_command(
      const toml::table& commands, const toml::value& definition, const std::string& name);
  void collect_commands(const toml::table& commands);
  void collect_params(const toml::table& params);

  const Path m_file_path;
  Commands m_commands{};
  Parameters m_parameters{};
};

}  // namespace litr::config
