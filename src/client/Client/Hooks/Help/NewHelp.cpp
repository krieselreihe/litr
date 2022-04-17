/*
 * Copyright (c) 2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "NewHelp.hpp"

#include <fmt/format.h>

#include "Client/Hooks/Help/HelpAllStrategy.hpp"
#include "Client/Hooks/Help/HelpCommandStrategy.hpp"

namespace Litr::Hook {

NewHelp::NewHelp(const std::shared_ptr<Config::Loader>& config,
    const std::shared_ptr<CLI::Instruction>& instruction,
    bool use_color)
    : Hook(config, instruction),
      m_query(config),
      m_use_color(use_color) {
  LITR_PROFILE_FUNCTION();

  const CLI::Interpreter interpreter{m_instruction, m_config};
  const std::string command_name{interpreter.command_name_occurrence({"h", "help"})};

  if (command_name.empty()) {
    m_help_strategy = std::make_unique<HelpAllStrategy>(config, instruction);
  } else {
    m_help_strategy = std::make_unique<HelpCommandStrategy>(config, instruction);
  }
}

std::string NewHelp::read() {
  LITR_PROFILE_FUNCTION();

  std::string help{welcome_message() + m_help_strategy->usage()};

  create_command_entries(m_help_strategy->commands());
  create_parameters_entries(m_help_strategy->parameters());
  define_left_margin();

  // Test built of the help string ...
  for (size_t i{0}; i != m_command_entries.size(); ++i) {
    auto& entry{m_command_entries[i]};
    const auto command{entry.reference};

    // No description
    if (command->description.empty()) {
      help += fmt::format("  {}\n", entry.value);
      continue;
    }

    // @todo: Get command_path
    // const std::string arguments{command_arguments(command_path)};

    // Remove line breaks
    const std::string description{StringUtils::replace(command->description, "\n", " ")};
    // 4 is twice the indentation of 2 whitespaces used before descriptions.
    const size_t description_line_length{
        command->description.length() + m_left_side_margin + 4};

    const size_t entry_value_length{StringUtils::utf8_length(entry.value)};

    // Short line.
    if (description_line_length <= M_MAX_LINE_LENGTH) {
      // Description on same line
      if (entry_value_length < M_MAX_DESCRIPTION_INSET) {
        help += fmt::format("  {:<{}}  {}\n", entry.value, m_left_side_margin, description);
        continue;
      }

      // Description on new line
      help += fmt::format(
          "  {}\n  │ {:<{}}{}\n", entry.value, line_padding(i), m_left_side_margin, description);
      continue;
    }

    // Multi line due to long line.
    const size_t max_length{M_MAX_LINE_LENGTH - m_left_side_margin};
    const std::vector<std::string> lines{StringUtils::split_by_whole_word(description, max_length)};

    for (size_t j{0}; j < lines.size(); ++j) {
      if (j == 0) {
        // Description on same line on first run
        if (entry_value_length < M_MAX_DESCRIPTION_INSET) {
          help += fmt::format("  {:<{}}  {}\n", entry.value, m_left_side_margin, lines[j]);
        } else {
          // Description on new line
          help += fmt::format(
              "  {}\n  │ {:<{}}{}\n", entry.value, line_padding(i), m_left_side_margin, lines[j]);
        }
      } else {
        help += fmt::format(
            "  │ {:<{}}{}\n", line_padding(i), m_left_side_margin, lines[j]);
      }
    }

    // Example
    if (!command->example.empty()) {
      std::vector<std::string> example_lines{};
      StringUtils::split_into(command->example, '\n', example_lines);

      help += format_with_color(fg(fmt::color::dark_gray), "  │ {:<{}}┌ Example(s):\n", line_padding(i), m_left_side_margin);
      for (auto&& line : example_lines) {
        if (line != example_lines.back()) {
          help += format_with_color(fg(fmt::color::dark_gray), "  │ {:<{}}│ {}\n", line_padding(i), m_left_side_margin, line);
        } else {
          help += format_with_color(fg(fmt::color::dark_gray), "  │ {:<{}}└ {}\n", line_padding(i), m_left_side_margin, line);
        }
      }
    }
  }

  // Parameters
  help += "\nOptions:\n";
  help += fmt::format("  {:<{}}  {}\n", "-h --help", m_left_side_margin, "Show this screen.");
  help += fmt::format("  {:<{}}  {}\n", "-v --version", m_left_side_margin, "Show current Litr version.");

  for (auto&& parameter : m_parameter_entries) {
    const auto param{parameter.reference};
    const size_t entry_value_length{StringUtils::utf8_length(parameter.value)};
    if (entry_value_length < M_MAX_DESCRIPTION_INSET) {
      // Same line
      help += fmt::format(
          "  {:<{}}  {}\n", parameter.value, m_left_side_margin, param->description);
    } else {
      // Next line
      help += fmt::format(
          "  {}\n    {:<{}}{}\n", parameter.value, "  ", m_left_side_margin, param->description);
    }

    // Available options
    if (!param->type_arguments.empty()) {
      std::string args{"Available options:"};
      for (auto&& arg : param->type_arguments) {
        args.append(fmt::format(" \"{}\",", arg));
      }

      help += format_with_color(
          fg(fmt::color::dark_gray), "  {:<{}}  {}\n", " ", m_left_side_margin,
          StringUtils::trim_right(args, ','));
    }

    // Default option
    if (!param->default_value.empty()) {
      help += format_with_color(fg(fmt::color::dark_gray),
          "  {:<{}}  {} \"{}\"\n",
          " ",
          m_left_side_margin,
          "Default option is:",
          param->default_value);
    }
  }

  return help;
}

// Needs to be recursive to traverse child commands.
// NOLINTNEXTLINE
void NewHelp::create_command_entries(const Config::Commands& commands, size_t depth) {
  LITR_PROFILE_FUNCTION();

  for (auto&& command : commands) {
    m_command_entries.emplace_back(command, depth);
    if (!command->child_commands.empty()) {
      create_command_entries(command->child_commands, depth + 1);
    }
  }
}

std::string NewHelp::command_arguments(const std::string& name) const {
  LITR_PROFILE_FUNCTION();

  Config::Parameters params{m_query.get_parameters(name)};
  std::string arguments{};

  std::sort(params.begin(), params.end(), sort_parameter_by_required);

  for (auto&& param : params) {
    switch (param->type) {
      case Config::Parameter::Type::STRING:
      case Config::Parameter::Type::ARRAY: {
        const bool is_optional{!param->default_value.empty()};
        const std::string pattern{is_optional ? " [--{}{}]" : " --{}{}"};
        arguments.append(
            fmt::format(is_optional ? fg(fmt::color::dark_gray) : fg(fmt::color::white),
                pattern,
                param->name,
                "=<option>"));
        break;
      }
      case Config::Parameter::Type::BOOLEAN: {
        arguments.append(fmt::format(fg(fmt::color::dark_gray), " [--{}]", param->name));
        break;
      }
    }
  }

  return arguments;
}

bool NewHelp::sort_parameter_by_required(const std::shared_ptr<Config::Parameter>& param1,
    const std::shared_ptr<Config::Parameter>& param2) {
  LITR_PROFILE_FUNCTION();

  auto rank_parameter_required{[](const std::shared_ptr<Config::Parameter>& param) -> int {
    switch (param->type) {
      case Config::Parameter::Type::STRING:
      case Config::Parameter::Type::ARRAY: {
        const bool is_required{param->default_value.empty()};
        return is_required ? 1 : 2;
      }
      case Config::Parameter::Type::BOOLEAN: {
        return 2;
      }
    }

    return -1;
  }};

  return rank_parameter_required(param1) < rank_parameter_required(param2);
}

void NewHelp::create_parameters_entries(const Config::Parameters& params) {
  LITR_PROFILE_FUNCTION();

  for (auto&& param : params) {
    m_parameter_entries.emplace_back(param);
  }
}

void NewHelp::define_left_margin() {
  LITR_PROFILE_FUNCTION();

  // Commands
  for (size_t i{0}; i != m_command_entries.size(); ++i) {
    auto& entry{m_command_entries[i]};

    if (entry.depth == 0) {
      entry.value = entry.reference->name;
    } else {
      const std::string line{same_depth_ahead(i + 1, entry.depth) ? "├ " : "└ "};
      entry.value = line_padding(i) + line + entry.reference->name;
    }

    const size_t entry_value_length{StringUtils::utf8_length(entry.value)};
    if (entry_value_length <= M_MAX_DESCRIPTION_INSET) {
      m_left_side_margin =
          m_left_side_margin >= entry_value_length ? m_left_side_margin : entry_value_length;
    }
  }

  // Parameters
  for (size_t i{0}; i != m_parameter_entries.size(); ++i) {
    auto& entry{m_parameter_entries[i]};

    const auto param{entry.reference};
    const std::string argument{param->type_arguments.empty() ? "" : "=<option>"};
    if (!param->shortcut.empty()) {
      entry.value = fmt::format("-{} --{}{}", param->shortcut, param->name, argument);
    } else {
      entry.value = fmt::format("   --{}{}", param->name, argument);
    }

    const size_t entry_value_length{StringUtils::utf8_length(entry.value)};
    if (entry_value_length <= M_MAX_DESCRIPTION_INSET) {
      m_left_side_margin =
          m_left_side_margin >= entry_value_length ? m_left_side_margin : entry_value_length;
    }
  }
}

std::string NewHelp::welcome_message() const {
  LITR_PROFILE_FUNCTION();

  std::string welcome_message{};

  welcome_message += fmt::format("Litr - Language Independent Task Runner [version {}.{}.{}]\n",
      LITR_VERSION_MAJOR,
      LITR_VERSION_MINOR,
      LITR_VERSION_PATCH);
  welcome_message += format_with_color(
      fg(fmt::color::dark_gray), "  Configuration file found under: {}\n\n", m_config->file_path());

  return welcome_message;
}

bool NewHelp::same_depth_ahead(size_t command_entry_start_index, size_t target_depth) const {
  LITR_PROFILE_FUNCTION();

  for (size_t i{command_entry_start_index}; i < m_command_entries.size(); ++i) {
    if (m_command_entries[i].depth == target_depth) {
      return true;
    }
  }
  return false;
}

std::string NewHelp::line_padding(size_t command_entry_index) const {
  LITR_PROFILE_FUNCTION();

  auto entry{m_command_entries[command_entry_index]};
  std::string padding{};

  // @todo: As soon as the nesting level reaches 3 this somehow breaks. There won't be more lines generated
  //  and all lines are only at the start. This is how this looks like:
  //
  //  release
  //  ├ brew               Build a production release for Homebrew on macOS.
  //  │ ├ another-one-command-super-long
  //  │ │                  Build a production release for Homebrew on macOS. With a very
  //  │ │                  long description, like really super duper long.
  //  │ └ next-level       Build a production release for Homebrew on macOS.
  //  │   ├ even-deeper    Build a production release for Homebrew on macOS. With a very
  //  │ │                  long description, like really super duper long. Also there
  //  │ │                  is.
  //  │   ├ even-deeper-as-well-as-long
  //  │ │                  Build a production release for Homebrew on macOS. With a very
  //  │ │                  long description, like really super duper long. Also there is
  //  │ │                  a line break in here.
  //  │   │ └ bob          Build a production release for Homebrew on macOS.
  //  │   └ even-deeper-2  Build a production release for Homebrew on macOS.
  //  └ another            Build another  production release for Something OS.
  //  format               Format project sources via clang-format.
  for (size_t i{1}; i < entry.depth; ++i) {
    padding += same_depth_ahead(command_entry_index + 1, i) ? "│ " : "  ";
  }

  return padding;
}

template <typename... Args>
std::string NewHelp::format_with_color(const fmt::text_style& ts, const Args&... args) const {
  LITR_PROFILE_FUNCTION();
  return fmt::format(m_use_color ? ts : fmt::text_style(), args...);
}

}  // namespace Litr::Hook
