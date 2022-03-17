/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Interpreter.hpp"

#include <algorithm>

#include "Core/CLI/Shell.hpp"
#include "Core/Debug/Instrumentor.hpp"
#include "Core/ExitStatus.hpp"
#include "Core/Script/Compiler.hpp"
#include "Core/Utils.hpp"

namespace litr::cli {

/** @private */
static void CommandPathToHumanReadable(std::string& path) {
  LITR_PROFILE_FUNCTION();

  std::replace(path.begin(), path.end(), '.', ' ');
}

Interpreter::Interpreter(
    const std::shared_ptr<Instruction>& instruction, const std::shared_ptr<config::Loader>& config)
    : m_instruction(instruction),
      m_query(config) {
  define_default_variables(config);
}

void Interpreter::execute() {
  LITR_PROFILE_FUNCTION();

  m_offset = 0;

  while (m_offset < m_instruction->count()) {
    if (m_stop_execution) {
      return;
    }
    execute_instruction();
  }
}

Instruction::Value Interpreter::read_current_value() const {
  LITR_PROFILE_FUNCTION();

  const std::byte index{m_instruction->read(m_offset)};
  return m_instruction->read_constant(index);
}

Interpreter::Variables Interpreter::get_scope_variables() const {
  LITR_PROFILE_FUNCTION();

  Variables variables{};

  for (auto&& scope : m_scope) {
    for (auto&& variable : scope) {
      variables.insert_or_assign(variable.first, variable.second);
    }
  }

  return variables;
}

void Interpreter::define_default_variables(const std::shared_ptr<config::Loader>& config) {
  LITR_PROFILE_FUNCTION();

  const auto params{config->get_parameters()};
  for (auto&& param : params) {
    switch (param->type) {
      case config::Parameter::Type::BOOLEAN: {
        Variable variable{param->name, false};
        m_scope.back().insert_or_assign(variable.name, variable);
        break;
      }
      case config::Parameter::Type::STRING:
      case config::Parameter::Type::ARRAY: {
        if (!param->default_value.empty()) {
          Variable variable{param->name, param->default_value};
          m_scope.back().insert_or_assign(variable.name, variable);
        }
        break;
      }
    }
  }
}

void Interpreter::execute_instruction() {
  LITR_PROFILE_FUNCTION();

  const Instruction::Code code{m_instruction->read(m_offset++)};

  switch (code) {
    case Instruction::Code::CLEAR:
      clear_scope();
      break;
    case Instruction::Code::DEFINE:
      define_variable();
      break;
    case Instruction::Code::CONSTANT:
      set_constant();
      break;
    case Instruction::Code::BEGIN_SCOPE:
      begin_scope();
      break;
    case Instruction::Code::EXECUTE:
      call_instruction();
      break;
    default:
      m_offset++;
  }
}

void Interpreter::begin_scope() {
  LITR_PROFILE_FUNCTION();

  m_scope.emplace_back(Variables());
  m_offset++;
}

void Interpreter::clear_scope() {
  LITR_PROFILE_FUNCTION();

  m_scope.pop_back();
}

void Interpreter::define_variable() {
  LITR_PROFILE_FUNCTION();

  const Instruction::Value name{read_current_value()};
  const std::shared_ptr<config::Parameter>& param{m_query.get_parameter(name)};

  if (param == nullptr) {
    handle_error(error::CommandNotFoundError(
        fmt::format("Parameter with the name \"{}\" is not defined."
                    "\n  Run `litr --help` to see a list available options.",
            name)));
    return;
  }

  Variable variable{get_variable_type(param), param->name};

  switch (param->type) {
    case config::Parameter::Type::BOOLEAN: {
      variable.value = true;
      break;
    }
    case config::Parameter::Type::STRING:
    case config::Parameter::Type::ARRAY: {
      if (!param->default_value.empty()) {
        variable.value = param->default_value;
      }
      break;
    }
  }

  m_current_variable_name = variable.name;
  m_scope.back().insert_or_assign(variable.name, variable);

  m_offset++;
}

void Interpreter::set_constant() {
  LITR_PROFILE_FUNCTION();

  const Instruction::Value value{read_current_value()};
  cli::Variable& variable{m_scope.back().at(m_current_variable_name)};
  const auto param{m_query.get_parameter(variable.name)};

  switch (param->type) {
    case config::Parameter::Type::STRING: {
      variable.value = value;
      break;
    }
    case config::Parameter::Type::ARRAY: {
      const auto& args{param->type_arguments};
      if (std::find(args.begin(), args.end(), value) == args.end()) {
        std::string options{"Available options are:"};
        for (auto&& option : args) {
          options.append(fmt::format(" \"{}\",", option));
        }

        handle_error(error::CommandNotFoundError(
            fmt::format("Parameter value \"{}\" is no valid option for \"{}\".\n  {}",
                value,
                param->name,
                utils::trim_right(options, ','))));
        return;
      }
      variable.value = value;
      break;
    }
    case config::Parameter::Type::BOOLEAN: {
      if (value == "false") {
        variable.value = false;
      } else if (value == "true") {
        variable.value = true;
      } else {
        handle_error(error::CommandNotFoundError(
            fmt::format("Parameter value \"{}\" is not valid for boolean option \"{}\"."
                        "\n  Please use \"false\", \"true\" or no value for true as well.",
                value,
                param->name)));
        return;
      }
      break;
    }
  }

  m_scope.back().insert_or_assign(variable.name, variable);
  m_offset++;
}

void Interpreter::call_instruction() {
  LITR_PROFILE_FUNCTION();

  const Instruction::Value name{read_current_value()};
  const std::shared_ptr<config::Command> command{m_query.get_command(name)};

  if (command == nullptr) {
    handle_error(error::CommandNotFoundError(fmt::format(
        "Command \"{}\" could not be found.\n  Run `litr --help` to see a list of commands.",
        name)));
    return;
  }

  call_command(command);
  m_offset++;
}

// Ignore recursive call of child commands.
// NOLINTNEXTLINE
void Interpreter::call_command(
    const std::shared_ptr<config::Command>& command, const std::string& scope) {
  LITR_PROFILE_FUNCTION();

  std::string command_path{scope + command->name};

  validate_required_parameters(command);
  if (m_stop_execution) {
    return;
  }

  const bool print_result{command->output == config::Command::Output::SILENT};
  const Scripts scripts{parse_scripts(command)};
  if (m_stop_execution) {
    return;
  }

  CommandPathToHumanReadable(command_path);

  if (command->directory.empty()) {
    run_scripts(scripts, command_path, "", print_result);
  } else {
    for (auto&& dir : command->directory) {
      run_scripts(scripts, command_path, dir, print_result);
    }
  }

  if (m_stop_execution) {
    return;
  }

  call_child_commands(command, command_path.append(" "));
}

// Ignore recursive call of child commands.
// NOLINTNEXTLINE
void Interpreter::call_child_commands(
    const std::shared_ptr<config::Command>& command, const std::string& scope) {
  LITR_PROFILE_FUNCTION();

  if (!command->child_commands.empty()) {
    for (auto&& child_command : command->child_commands) {
      if (m_stop_execution) {
        return;
      }
      call_command(child_command, scope);
    }
  }
}

void Interpreter::run_scripts(const Scripts& scripts,
    const std::string& command_path,
    const std::string& dir,
    bool print_result) {
  LITR_PROFILE_FUNCTION();

  Path path{dir};

  for (auto&& script : scripts) {
    Shell::Result result{
        print_result ? Shell::exec(script, path) : Shell::exec(script, path, print)};

    if (result.status == ExitStatus::FAILURE) {
      handle_error(error::ExecutionFailureError(
          fmt::format("Problem executing the command defined in \"{}\".", command_path)));
      return;
    }
  }
}

Interpreter::Scripts Interpreter::parse_scripts(const std::shared_ptr<config::Command>& command) {
  LITR_PROFILE_FUNCTION();

  size_t location{0};
  Scripts scripts{};

  for (auto&& script : command->script) {
    const std::string parsed_script{parse_script(script, command->Locations[location])};
    if (m_stop_execution) {
      break;
    }
    scripts.push_back(parsed_script);
    location++;
  }

  return scripts;
}

std::string Interpreter::parse_script(const std::string& script, const config::Location& location) {
  LITR_PROFILE_FUNCTION();

  const Variables variables{get_scope_variables()};
  script::Compiler parser{script, location, variables};

  if (error::Handler::has_errors()) {
    m_stop_execution = true;
  }

  return parser.get_script();
}

enum Variable::Type Interpreter::get_variable_type(
    const std::shared_ptr<config::Parameter>& param) {
  LITR_PROFILE_FUNCTION();

  switch (param->type) {
    case config::Parameter::Type::BOOLEAN: {
      return Variable::Type::BOOLEAN;
    }
    case config::Parameter::Type::STRING:
    case config::Parameter::Type::ARRAY: {
      return Variable::Type::STRING;
    }
  }

  return {};
}

void Interpreter::validate_required_parameters(const std::shared_ptr<config::Command>& command) {
  LITR_PROFILE_FUNCTION();

  const auto params{m_query.get_parameters(command->name)};

  for (auto&& param : params) {
    if (!is_variable_defined(param->name)) {
      handle_error(error::ExecutionFailureError(
          fmt::format("The parameter --{} is required. "
                      "You should run the command again with the required parameter.",
              param->name)));
      m_stop_execution = true;
    }
  }
}

bool Interpreter::is_variable_defined(const std::string& name) const {
  LITR_PROFILE_FUNCTION();

  const Variables variables{get_scope_variables()};
  return std::any_of(
      variables.begin(), variables.end(), [&name](std::pair<std::string, cli::Variable>&& var) {
        if (var.second.type == cli::Variable::Type::STRING) {
          return name == var.second.name && !std::get<std::string>(var.second.value).empty();
        }

        return name == var.second.name;
      });
}

void Interpreter::handle_error(const error::BaseError& error) {
  LITR_PROFILE_FUNCTION();

  m_stop_execution = true;
  error::Handler::push(error);
}

void Interpreter::print(const std::string& message) {
  LITR_PROFILE_FUNCTION();

  fmt::print("{}", message);
}

}  // namespace litr::cli
