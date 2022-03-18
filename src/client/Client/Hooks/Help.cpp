/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Help.hpp"

#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/printf.h>

#include <algorithm>
#include <vector>

namespace litr::hook {

// Attention dear reader: I know how this file looks like. There is a lot going on,
// numbers, formatting, some of it all over the place. It is not pretty, but it works
// and very important the mess is contained to this file.
// There will be a refactor for this making some improvements on the way as well:
// https://github.com/krieselreihe/litr/issues/31

Help::Help(const std::shared_ptr<config::Loader>& config)
    : m_query(config),
      m_file_path(config->get_file_path()) {}

void Help::print(const std::shared_ptr<cli::Instruction>& instruction) const {
  LITR_PROFILE_FUNCTION();

  m_command_name = get_command_name(instruction);

  print_welcome_message();
  print_usage();
  print_commands();
  print_options();
}

void Help::print_welcome_message() const {
  LITR_PROFILE_FUNCTION();

  fmt::print("Litr - Language Independent Task Runner [version {}.{}.{}]\n",
      LITR_VERSION_MAJOR,
      LITR_VERSION_MINOR,
      LITR_VERSION_PATCH);
  fmt::print(fg(fmt::color::dark_gray), "  Configuration file found under: {}\n\n", m_file_path);
}

void Help::print_usage() const {
  LITR_PROFILE_FUNCTION();

  if (!m_command_name.empty()) {
    std::string command_name{m_command_name};
    std::replace(command_name.begin(), command_name.end(), '.', ' ');
    fmt::print("Usage: litr {} [options]\n\n", command_name);
    return;
  }

  fmt::print("Usage: litr command [options]\n\n");
}

void Help::print_commands() const {
  LITR_PROFILE_FUNCTION();

  const config::Query::Commands commands{
      m_command_name.empty() ? m_query.get_commands() : m_query.get_commands(m_command_name)};

  if (commands.empty()) {
    return;
  }

  fmt::print("Commands:\n");
  for (auto&& command : commands) {
    print_command(command, m_command_name);
  }
  fmt::print("\n");
}

void Help::print_command(const std::shared_ptr<config::Command>& command,
    const std::string& parent_name,
    size_t depth) const {
  size_t padding{get_command_padding()};
  std::string command_path{command->name};

  if (!parent_name.empty()) {
    padding -= (depth - 1) * 2;  // Reduce right padding for short child commands.
    command_path = fmt::format("{}.{}", parent_name, command->name);
  }

  const std::string arguments{get_command_arguments(command_path)};
  const std::string name{fmt::format("{: ^{}}{:<{}} {}",
      "",
      depth * 2,  // Left side padding
      command->name,
      padding,
      arguments)};

  if (command->description.empty()) {
    fmt::print("{}\n", name);
  } else {
    print_with_description(name, fmt::format("  {}", command->description), arguments.length());
  }

  print_example(command);

  for (auto&& child_command : command->child_commands) {
    print_command(child_command, command_path, depth + 1);
  }
}

void Help::print_example(const std::shared_ptr<config::Command>& command) const {
  if (!command->example.empty()) {
    const size_t padding{get_parameter_padding()};

    std::vector<std::string> lines{};
    utils::split_into(command->example, '\n', lines);

    fmt::print(fg(fmt::color::dark_gray), "{:<{}}   ┌ Example(s):\n", " ", padding);
    for (auto&& line : lines) {
      if (line != lines.back()) {
        fmt::print(fg(fmt::color::dark_gray), "{:<{}}   │ {}\n", " ", padding, line);
      } else {
        fmt::print(fg(fmt::color::dark_gray), "{:<{}}   └ {}\n", " ", padding, line);
      }
    }
  }
}

void Help::print_options() const {
  LITR_PROFILE_FUNCTION();

  const config::Query::Parameters params{
      m_command_name.empty() ? m_query.get_parameters() : m_query.get_parameters(m_command_name)};
  const size_t padding{get_parameter_padding()};

  fmt::print("Options:\n");
  fmt::print("  {:<{}} {}\n", "-h --help", padding, "Show this screen.");
  fmt::print("  {:<{}} {}\n", "-v --version", padding, "Show current Litr version.");

  for (auto&& param : params) {
    std::string name{};
    const std::string argument{param->type_arguments.empty() ? "" : m_argument_placeholder};

    // Parameter names
    if (!param->shortcut.empty()) {
      name = fmt::format("-{} --{}{}", param->shortcut, param->name, argument);
    } else {
      name = fmt::format("   --{}{}", param->name, argument);
    }

    fmt::print("  ");
    print_with_description(name, param->description);
    print_parameter_options(param);
    print_default_parameter_option(param);
  }
}

void Help::print_parameter_options(const std::shared_ptr<config::Parameter>& param) const {
  LITR_PROFILE_FUNCTION();

  if (!param->type_arguments.empty()) {
    std::string args{"Available options:"};
    const size_t padding{get_parameter_padding()};

    for (auto&& arg : param->type_arguments) {
      args.append(fmt::format(" \"{}\",", arg));
    }

    fmt::print(
        fg(fmt::color::dark_gray), "  {:<{}} {}\n", " ", padding, utils::trim_right(args, ','));
  }
}

void Help::print_default_parameter_option(const std::shared_ptr<config::Parameter>& param) const {
  LITR_PROFILE_FUNCTION();

  if (!param->default_value.empty()) {
    const size_t padding{get_parameter_padding()};
    fmt::print(fg(fmt::color::dark_gray),
        "  {:<{}} {} \"{}\"\n",
        " ",
        padding,
        "Default option is:",
        param->default_value);
  }
}

void Help::print_with_description(
    const std::string& name, const std::string& description, size_t extra_padding) const {
  LITR_PROFILE_FUNCTION();

  std::vector<std::string> lines{};
  utils::split_into(description, '\n', lines);

  const size_t max_padding{22};
  const size_t padding{get_parameter_padding()};
  bool use_max_padding{padding + extra_padding > max_padding};

  for (size_t i{0}; i < lines.size(); ++i) {
    if (use_max_padding) {
      if (i == 0) {
        fmt::print("{}\n", name);
      }
      fmt::print("{:<{}} {}\n", " ", padding, lines[i]);
    } else {
      if (i == 0) {
        fmt::print("{:<{}} {}\n", name, padding, lines[i]);
      } else {
        fmt::print("{:<{}}   {}\n", " ", padding, lines[i]);
      }
    }
  }
}

std::string Help::get_command_name(const std::shared_ptr<cli::Instruction>& instruction) {
  LITR_PROFILE_FUNCTION();

  size_t offset{0};
  std::vector<std::string> scope{};

  while (offset < instruction->count()) {
    const cli::Instruction::Code code{instruction->read(offset++)};

    switch (code) {
      case cli::Instruction::Code::BEGIN_SCOPE: {
        const std::string value{instruction->read_constant(instruction->read(offset))};
        scope.push_back(value);
        ++offset;
        break;
      }
      case cli::Instruction::Code::CLEAR: {
        scope.pop_back();
        break;
      }
      case cli::Instruction::Code::DEFINE: {
        const std::string value{instruction->read_constant(instruction->read(offset))};
        if (value == "h" || value == "help") {
          std::string command_name{};
          for (auto&& part : scope) {
            command_name.append(".").append(part);
          }
          return utils::trim_left(command_name, '.');
        }
        ++offset;
        break;
      }
      case cli::Instruction::Code::CONSTANT:
      case cli::Instruction::Code::EXECUTE: {
        ++offset;
        break;
      }
    }
  }

  return "";
}

std::string Help::get_command_arguments(const std::string& name) const {
  LITR_PROFILE_FUNCTION();

  config::Query::Parameters params{m_query.get_parameters(name)};
  std::string arguments{};

  std::sort(params.begin(), params.end(), sort_parameter_by_required);

  for (auto&& param : params) {
    switch (param->type) {
      case config::Parameter::Type::STRING:
      case config::Parameter::Type::ARRAY: {
        const bool is_optional{!param->default_value.empty()};
        const std::string pattern{is_optional ? " [--{}{}]" : " --{}{}"};
        arguments.append(
            fmt::format(is_optional ? fg(fmt::color::dark_gray) : fg(fmt::color::white),
                pattern,
                param->name,
                m_argument_placeholder));
        break;
      }
      case config::Parameter::Type::BOOLEAN: {
        arguments.append(fmt::format(fg(fmt::color::dark_gray), " [--{}]", param->name));
        break;
      }
    }
  }

  return arguments;
}

size_t Help::get_command_padding() const {
  LITR_PROFILE_FUNCTION();

  const config::Query::Commands commands{
      m_command_name.empty() ? m_query.get_commands() : m_query.get_commands(m_command_name)};
  return get_command_padding(commands);
}

size_t Help::get_command_padding(const config::Query::Commands& commands) const {
  LITR_PROFILE_FUNCTION();

  size_t padding{0};

  for (auto&& command : commands) {
    const size_t command_length{command->name.length()};

    if (padding < command_length) {
      padding = command_length;
    }

    // Add 2 for child command padding alignment. It's a hack, I know ¯\_(ツ)_/¯
    const size_t child_padding{get_command_padding(command->child_commands) + 2};

    if (padding < child_padding) {
      padding = child_padding;
    }
  }

  return padding;
}

size_t Help::get_parameter_padding() const {
  LITR_PROFILE_FUNCTION();

  const config::Query::Parameters params{
      m_command_name.empty() ? m_query.get_parameters() : m_query.get_parameters(m_command_name)};
  size_t padding{0};

  constexpr size_t min_padding{7};   // Min padding based on text "version"
  constexpr size_t padding_left{4};  // Includes space for short parameter
  constexpr size_t padding_right{2};

  for (auto&& param : params) {
    const std::string argument{param->type_arguments.empty() ? "" : m_argument_placeholder};
    size_t param_length{(param->name.length() + argument.length())};

    if (padding < param_length) {
      padding = param_length;
    }
  }

  if (padding < min_padding) {
    padding = min_padding;
  }

  return padding_left + padding + padding_right;
}

bool Help::sort_parameter_by_required(
    const std::shared_ptr<config::Parameter>& p1, const std::shared_ptr<config::Parameter>& p2) {
  LITR_PROFILE_FUNCTION();

  auto rank_parameter_required{[](const std::shared_ptr<config::Parameter>& param) -> int {
    switch (param->type) {
      case config::Parameter::Type::STRING:
      case config::Parameter::Type::ARRAY: {
        const bool is_required{param->default_value.empty()};
        return is_required ? 1 : 2;
      }
      case config::Parameter::Type::BOOLEAN: {
        return 2;
      }
    }

    return -1;
  }};

  return rank_parameter_required(p1) < rank_parameter_required(p2);
}

}  // namespace litr::hook
