/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>
#include <vector>

#include <toml.hpp>

#include "Core/Base.hpp"
#include "Core/Config/Command.hpp"
#include "Core/FileSystem.hpp"

namespace litr::Config {

class CommandBuilder {
 public:
  CommandBuilder(const toml::table& file, const toml::value& data, const std::string& name);

  void AddScriptLine(const std::string& line);
  void AddScriptLine(const std::string& line, const toml::value& context);
  void AddScript(const std::vector<std::string>& scripts);
  void AddScript(const std::vector<std::string>& scripts, const toml::value& context);
  void AddScript(const toml::value& scripts);

  void AddDescription();
  void AddExample();
  void AddDirectory(const Path& root);
  void AddOutput();
  void AddChildCommand(const Ref<Command>& command);

  [[nodiscard]] inline Ref<Command> GetResult() const { return m_Command; }

 private:
  void AddLocation(const toml::value& context);

 private:
  const toml::table& m_File;
  const toml::value& m_Table;
  const Ref<Command> m_Command;
};

}  // namespace litr::Config
