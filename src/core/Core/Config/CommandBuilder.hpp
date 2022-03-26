/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <tsl/ordered_map.h>

#include <memory>
#include <string>
#include <vector>

#include "Core/Config/Command.hpp"
#include "Core/Config/TomlFileAdapter.hpp"
#include "Core/FileSystem.hpp"

namespace litr::config {

class CommandBuilder {
 public:
  CommandBuilder(const TomlFileAdapter::Value& context,
      const TomlFileAdapter::Value& data,
      const std::string& name);

  void add_script_line(const std::string& line);
  void add_script_line(const std::string& line, const TomlFileAdapter::Value& context);
  void add_script(const std::vector<std::string>& scripts);
  void add_script(const TomlFileAdapter::Value& scripts);

  void add_description();
  void add_example();
  void add_directory(const Path& root);
  void add_output();
  void add_child_command(const std::shared_ptr<Command>& command);

  [[nodiscard]] inline std::shared_ptr<Command> get_result() const {
    return m_command;
  }

 private:
  void add_location(const TomlFileAdapter::Value& context);

  const TomlFileAdapter::Value& m_context;
  const TomlFileAdapter::Value& m_table;
  const std::shared_ptr<Command> m_command;
  const TomlFileAdapter m_file{};
};

}  // namespace litr::config
