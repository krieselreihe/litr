/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <memory>
#include <string>
#include <toml.hpp>
#include <vector>

#include "Core/Config/Command.hpp"
#include "Core/FileSystem.hpp"

namespace litr::config {

class CommandBuilder {
 public:
  CommandBuilder(const toml::table& file, const toml::value& data, const std::string& name);

  void add_script_line(const std::string& line);
  void add_script_line(const std::string& line, const toml::value& context);
  void add_script(const std::vector<std::string>& scripts);
  void add_script(const toml::value& scripts);

  void add_description();
  void add_example();
  void add_directory(const Path& root);
  void add_output();
  void add_child_command(const std::shared_ptr<Command>& command);

  [[nodiscard]] inline std::shared_ptr<Command> get_result() const {
    return m_command;
  }

 private:
  void add_location(const toml::value& context);

 private:
  const toml::table& m_file;
  const toml::value& m_table;
  const std::shared_ptr<Command> m_command;
};

}  // namespace litr::config
