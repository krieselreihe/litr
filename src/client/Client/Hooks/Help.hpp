/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>

#include "Core.hpp"

namespace Litr::Hook {

class Help {
 public:
  explicit Help(const std::shared_ptr<Config::Loader>& config);

  void print(const std::shared_ptr<CLI::Instruction>& instruction) const;

 private:
  void print_welcome_message() const;
  void print_usage() const;
  void print_command_usage() const;

  void print_commands() const;
  void print_command(const std::shared_ptr<Config::Command>& command,
      const std::string& parent_name,
      size_t depth = 1) const;

  void print_example(const std::shared_ptr<Config::Command>& command) const;
  void print_options() const;
  void print_parameter_options(const std::shared_ptr<Config::Parameter>& param) const;
  void print_default_parameter_option(const std::shared_ptr<Config::Parameter>& param) const;
  void print_with_description(
      const std::string& name, const std::string& description, size_t extra_padding = 0) const;

  [[nodiscard]] static std::string get_command_name(
      const std::shared_ptr<CLI::Instruction>& instruction);
  [[nodiscard]] std::string get_command_arguments(const std::string& name) const;

  [[nodiscard]] size_t get_command_padding() const;
  [[nodiscard]] size_t get_command_padding(const Config::Query::Commands& commands) const;
  [[nodiscard]] size_t get_parameter_padding() const;

  [[nodiscard]] static bool sort_parameter_by_required(
      const std::shared_ptr<Config::Parameter>& param1,
      const std::shared_ptr<Config::Parameter>& param2);

  const Config::Query m_query;
  const Path m_file_path;

  mutable std::string m_command_name{};
  const std::string m_argument_placeholder{"=<option>"};
};

}  // namespace Litr::Hook
