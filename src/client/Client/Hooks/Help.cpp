#include "Help.hpp"

#include <algorithm>
#include <fmt/color.h>
#include <fmt/printf.h>
#include <fmt/format.h>

namespace Litr {

Help::Help(const Ref<Config::Loader>& config) : m_Query(config), m_FilePath(config->GetFilePath()) {
  SetGlobalPadding();
}

void Help::Print(const Ref<CLI::Instruction>& instruction) const {
  LITR_PROFILE_FUNCTION();

  m_CommandName = GetCommandName(instruction);

  PrintWelcomeMessage();
  PrintUsage();
  PrintCommands();
  PrintOptions();
}

void Help::PrintWelcomeMessage() const {
  LITR_PROFILE_FUNCTION();

  fmt::print(
      "Litr - Language Independent Task Runner [version {}.{}.{}]\n",
      LITR_VERSION_MAJOR, LITR_VERSION_MINOR, LITR_VERSION_PATCH);
  fmt::print(fg(fmt::color::dark_gray), "  Configuration file found under: {}\n\n", m_FilePath);
}

void Help::PrintUsage() const {
  if (!m_CommandName.empty()) {
    std::string commandName{m_CommandName};
    std::replace(commandName.begin(), commandName.end(), '.', ' ');
    fmt::print("Usage: litr {} [options]\n\n", commandName);
    return;
  }

  fmt::print("Usage: litr command [options]\n\n");
}

void Help::PrintCommands() const {
  LITR_PROFILE_FUNCTION();

  if (!m_CommandName.empty()) return;

  const Config::Query::Commands commands{m_Query.GetCommands()};

  fmt::print("Commands:\n");

  for (auto&& command : commands) {
    const std::string arguments{GetCommandArguments(command->Name)};
    const std::string name{fmt::format("{:<{}} {}", command->Name, m_CommandPadding, arguments)};

    if (command->Description.empty()) {
      fmt::print("  {}\n", name);
    } else {
      PrintWithDescription(name, command->Description, arguments.length());
    }
  }

  fmt::print("\n");
}

void Help::PrintOptions() const {
  LITR_PROFILE_FUNCTION();

  const Config::Query::Parameters params{
      m_CommandName.empty() ? m_Query.GetParameters() : m_Query.GetCommandParameters(m_CommandName)};

  fmt::print("Options:\n");
  fmt::print("  {:<{}} {}\n", "-h --help", m_Padding, "Show this screen.");

  for (auto&& param : params) {
    std::string name{};
    std::string argument{param->TypeArguments.empty() ? "" : m_ArgumentPlaceholder};

    // Parameter names
    if (!param->Shortcut.empty()) {
      name = fmt::format("-{} --{}{}", param->Shortcut, param->Name, argument);
    } else {
      name = fmt::format("   --{}{}", param->Name, argument);
    }

    PrintWithDescription(name, param->Description);
    PrintOptions(param);
    PrintDefaultOption(param);
  }
}

void Help::PrintOptions(const Ref<Config::Parameter>& param) const {
  if (!param->TypeArguments.empty()) {
    std::string args{"Available options:"};

    for (auto&& arg : param->TypeArguments) {
      args.append(fmt::format(" \"{}\",", arg));
    }

    fmt::print(
        fg(fmt::color::dark_gray),
        "  {:<{}} {}\n", " ", m_Padding, Utils::TrimRight(args, ','));
  }
}

void Help::PrintDefaultOption(const Ref<Config::Parameter>& param) const {
  if (!param->Default.empty()) {
    fmt::print(
        fg(fmt::color::dark_gray),
        "  {:<{}} {} \"{}\"\n", " ", m_Padding, "Default option is:", param->Default);
  }
}

void Help::PrintWithDescription(const std::string& name, const std::string& description, size_t extraPadding) const {
  LITR_PROFILE_FUNCTION();

  std::vector<std::string> lines{};
  Utils::SplitInto(description, '\n', lines);

  size_t maxPadding{22};
  bool useMaxPadding{m_Padding + extraPadding > maxPadding};

  for (size_t i{0}; i < lines.size(); ++i) {
    if (useMaxPadding) {
      if (i == 0) fmt::print("  {}\n", name);
      fmt::print("  {:<{}} {}\n", " ", m_Padding, lines[i]);
    } else {
      if (i == 0) {
        fmt::print("  {:<{}} {}\n", name, m_Padding, lines[i]);
      } else {
        fmt::print("  {:<{}} {}\n", " ", m_Padding, lines[i]);
      }
    }
  }
}

std::string Help::GetCommandName(const Ref<CLI::Instruction>& instruction) {
  size_t offset{0};
  std::vector<std::string> scope{};

  while (offset < instruction->Count()) {
    const auto code{static_cast<CLI::Instruction::Code>(instruction->Read(offset++))};
    switch (code) {
      case CLI::Instruction::Code::BEGIN_SCOPE: {
        auto value{instruction->ReadConstant(instruction->Read(offset))};
        scope.push_back(value);
        offset++;
        break;
      }
      case CLI::Instruction::Code::CLEAR: {
        scope.pop_back();
        break;
      }
      case CLI::Instruction::Code::DEFINE: {
        auto value{instruction->ReadConstant(instruction->Read(offset))};
        if (value == "h" || value == "help") {
          std::string commandName{};
          for (auto&& part : scope) commandName.append(".").append(part);
          return Utils::TrimLeft(commandName, '.');
        }
        offset++;
        break;
      }
      case CLI::Instruction::Code::CONSTANT:
      case CLI::Instruction::Code::EXECUTE: {
        offset++;
        break;
      }
    }
  }

  return "";
}

std::string Help::GetCommandArguments(const std::string& name) const {
  // @todo: Needs to be sorted by required/optional parameters.
  const Config::Query::Parameters params{m_Query.GetCommandParameters(name)};
  std::string arguments{};

  for (auto&& param : params) {
    switch (param->Type) {
      case Config::Parameter::Type::STRING:
      case Config::Parameter::Type::ARRAY: {
        bool isOptional{!param->Default.empty()};
        const std::string pattern{isOptional ? " [--{}{}]" : " --{}{}"};
        arguments.append(fmt::format(
            isOptional ? fg(fmt::color::dark_gray) : fg(fmt::color::white),
            pattern, param->Name, m_ArgumentPlaceholder));
        break;
      }
      case Config::Parameter::Type::BOOLEAN: {
        arguments.append(fmt::format(fg(fmt::color::dark_gray)," [--{}]", param->Name));
        break;
      }
    }
  }

  return arguments;
}

void Help::SetGlobalPadding() {
  LITR_PROFILE_FUNCTION();

  const size_t paddingRight{2};
  const size_t paddingLeft{4};  // Includes space for short parameter

  const size_t commandPadding{GetCommandPadding()};
  const size_t paramPadding{GetParameterPadding()};

  size_t padding{commandPadding};

  m_CommandPadding = padding;

  if (paramPadding > padding) {
    padding = paramPadding;
  }

  m_Padding = paddingLeft + padding + paddingRight;
}

size_t Help::GetCommandPadding() const {
  LITR_PROFILE_FUNCTION();

  const Config::Query::Commands commands{m_Query.GetCommands()};
  size_t padding{0};

  for (auto&& command : commands) {
    size_t commandLength{command->Name.length()};

    if (padding < commandLength) {
      padding = commandLength;
    }
  }

  return padding;
}

size_t Help::GetParameterPadding() const {
  LITR_PROFILE_FUNCTION();

  const Config::Query::Parameters params{m_Query.GetParameters()};
  size_t padding{0};

  for (auto&& param : params) {
    std::string argument{param->TypeArguments.empty() ? "" : m_ArgumentPlaceholder};
    size_t paramLength{(param->Name.length() + argument.length())};

    if (padding < paramLength) {
      padding = paramLength;
    }
  }

  return padding;
}

}  // namespace Litr
