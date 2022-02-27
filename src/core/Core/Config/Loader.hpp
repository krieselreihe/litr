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

namespace litr::config {

class Loader {
 public:
  using Commands = std::vector<Ref<Command>>;
  using Parameters = std::vector<Ref<Parameter>>;

  explicit Loader(const Path& file_path);
  ~Loader() = default;

  [[nodiscard]] inline Commands get_commands() const { return m_commands; }
  [[nodiscard]] inline Parameters get_parameters() const { return m_parameters; }
  [[nodiscard]] inline Path get_file_path() const { return m_file_path; }

 private:
  Ref<Command> create_command(const toml::table& commands, const toml::value& definition, const std::string& name);
  void collect_commands(const toml::table& commands);
  void collect_params(const toml::table& params);

  const Path m_file_path;
  Commands m_commands{};
  Parameters m_parameters{};
};

}  // namespace litr::config
