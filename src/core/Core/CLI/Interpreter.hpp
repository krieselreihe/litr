/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <memory>

#include "Core/CLI/Instruction.hpp"
#include "Core/CLI/Variable.hpp"
#include "Core/Config/Loader.hpp"
#include "Core/Config/Location.hpp"
#include "Core/Config/Query.hpp"
#include "Core/Error/Handler.hpp"

namespace litr::cli {

class Interpreter {
  using Variables = std::unordered_map<std::string, cli::Variable>;
  using Scripts = std::vector<std::string>;

 public:
  Interpreter(const std::shared_ptr<Instruction>& instruction, const std::shared_ptr<config::Loader>& config);

  void execute();

 private:
  [[nodiscard]] Instruction::Value read_current_value() const;
  [[nodiscard]] Variables get_scope_variables() const;
  void define_default_variables(const std::shared_ptr<config::Loader>& config);

  void execute_instruction();

  void begin_scope();
  void clear_scope();
  void define_variable();
  void set_constant();
  void call_instruction();

  void call_command(const std::shared_ptr<config::Command>& command, const std::string& scope = "");
  void call_child_commands(const std::shared_ptr<config::Command>& command, const std::string& scope);
  void run_scripts(const Scripts& scripts, const std::string& command_path, const std::string& dir, bool print_result);

  [[nodiscard]] Scripts parse_scripts(const std::shared_ptr<config::Command>& command);
  [[nodiscard]] std::string parse_script(const std::string& script, const config::Location& location);

  [[nodiscard]] static enum Variable::Type get_variable_type(const std::shared_ptr<config::Parameter>& param);

  void validate_required_parameters(const std::shared_ptr<config::Command>& command);
  [[nodiscard]] bool is_variable_defined(const std::string& name) const;
  void handle_error(const error::BaseError& error);

  static void print(const std::string& message);

 private:
  const std::shared_ptr<Instruction>& m_instruction;
  const config::Query m_query;

  size_t m_offset{0};
  std::string m_current_variable_name{};
  bool m_stop_execution{false};

  // Initialize with empty scope
  std::vector<Variables> m_scope{Variables()};
};

}  // namespace litr::cli
