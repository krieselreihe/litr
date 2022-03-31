/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <deque>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Core/CLI/Variable.hpp"
#include "Core/Config/Loader.hpp"

namespace Litr::Config {

class Query {
  using Parts = std::deque<std::string>;
  using Variables = std::unordered_map<std::string, CLI::Variable>;

 public:
  using Commands = std::vector<std::shared_ptr<Command>>;
  using Parameters = std::vector<std::shared_ptr<Parameter>>;

  explicit Query(const std::shared_ptr<Loader>& config);

  [[nodiscard]] std::shared_ptr<Command> get_command(const std::string& name) const;
  [[nodiscard]] std::shared_ptr<Parameter> get_parameter(const std::string& name) const;

  [[nodiscard]] Commands get_commands() const;
  [[nodiscard]] Commands get_commands(const std::string& name) const;
  [[nodiscard]] Parameters get_parameters() const;
  [[nodiscard]] Parameters get_parameters(const std::string& command_name) const;

 private:
  [[nodiscard]] static Parts split_command_query(const std::string& query);
  [[nodiscard]] static std::shared_ptr<Command> get_command_by_path(
      Parts& names, const Loader::Commands& commands);
  [[nodiscard]] static std::shared_ptr<Command> get_command_by_name(
      const std::string& name, const Loader::Commands& commands);

  [[nodiscard]] Variables get_parameters_as_variables() const;
  [[nodiscard]] std::vector<std::string> get_used_parameter_names(
      const std::shared_ptr<Command>& command) const;

  const std::shared_ptr<Loader>& m_config;
};

}  // namespace Litr::Config
