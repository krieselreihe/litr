#include "Help.hpp"

#include <algorithm>
#include <fmt/color.h>
#include <fmt/printf.h>
#include <fmt/format.h>

namespace Litr::Hook {

// Attention dear reader: I know how this file looks like. There is a lot going on,
// numbers, formatting, some of it all over the place. It is not pretty, but it works
// and very important the mess is contained to this file.
// There will be a refactor for this making some improvements on the way as well:
// https://github.com/krieselreihe/litr/issues/31

Help::Help(const Ref<Config::Loader>& config) : m_Query(config), m_FilePath(config->GetFilePath()) {}

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
  LITR_PROFILE_FUNCTION();

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

  const Config::Query::Commands commands{
      m_CommandName.empty() ? m_Query.GetCommands() : m_Query.GetCommands(m_CommandName)};

  if (commands.empty()) return;

  fmt::print("Commands:\n");
  for (auto&& command : commands) PrintCommand(command, m_CommandName);
  fmt::print("\n");
}

void Help::PrintCommand(const Ref<Config::Command>& command, const std::string& parentName, size_t depth) const {
  size_t padding{GetCommandPadding()};
  std::string commandPath{command->Name};

  if (!parentName.empty()) {
    padding -= (depth - 1) * 2;  // Reduce right padding for short child commands.
    commandPath = fmt::format("{}.{}", parentName, command->Name);
  }

  const std::string arguments{GetCommandArguments(commandPath)};
  const std::string name{fmt::format("{: ^{}}{:<{}} {}",
      "", depth * 2,  // Left side padding
      command->Name, padding, arguments)};

  if (command->Description.empty()) {
    fmt::print("{}\n", name);
  } else {
    PrintWithDescription(name, fmt::format("  {}", command->Description), arguments.length());
  }

  PrintExample(command);

  for (auto&& childCommand : command->ChildCommands) {
    PrintCommand(childCommand, commandPath, depth + 1);
  }
}

void Help::PrintExample(const Ref<Config::Command>& command) const {
  if (!command->Example.empty()) {
    const size_t padding{GetParameterPadding()};

    std::vector<std::string> lines{};
    Utils::SplitInto(command->Example, '\n', lines);

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

void Help::PrintOptions() const {
  LITR_PROFILE_FUNCTION();

  const Config::Query::Parameters params{
      m_CommandName.empty() ? m_Query.GetParameters() : m_Query.GetParameters(m_CommandName)};
  const size_t padding{GetParameterPadding()};

  fmt::print("Options:\n");
  fmt::print("  {:<{}} {}\n", "-h --help", padding, "Show this screen.");
  fmt::print("  {:<{}} {}\n", "-v --version", padding, "Show current Litr version.");

  for (auto&& param : params) {
    std::string name{};
    const std::string argument{param->TypeArguments.empty() ? "" : m_ArgumentPlaceholder};

    // Parameter names
    if (!param->Shortcut.empty()) {
      name = fmt::format("-{} --{}{}", param->Shortcut, param->Name, argument);
    } else {
      name = fmt::format("   --{}{}", param->Name, argument);
    }

    fmt::print("  ");
    PrintWithDescription(name, param->Description);
    PrintParameterOptions(param);
    PrintDefaultParameterOption(param);
  }
}

void Help::PrintParameterOptions(const Ref<Config::Parameter>& param) const {
  LITR_PROFILE_FUNCTION();

  if (!param->TypeArguments.empty()) {
    std::string args{"Available options:"};
    const size_t padding{GetParameterPadding()};

    for (auto&& arg : param->TypeArguments) {
      args.append(fmt::format(" \"{}\",", arg));
    }

    fmt::print(
        fg(fmt::color::dark_gray),
        "  {:<{}} {}\n", " ", padding, Utils::TrimRight(args, ','));
  }
}

void Help::PrintDefaultParameterOption(const Ref<Config::Parameter>& param) const {
  LITR_PROFILE_FUNCTION();

  if (!param->Default.empty()) {
    const size_t padding{GetParameterPadding()};
    fmt::print(
        fg(fmt::color::dark_gray),
        "  {:<{}} {} \"{}\"\n", " ", padding, "Default option is:", param->Default);
  }
}

void Help::PrintWithDescription(const std::string& name, const std::string& description, size_t extraPadding) const {
  LITR_PROFILE_FUNCTION();

  std::vector<std::string> lines{};
  Utils::SplitInto(description, '\n', lines);

  const size_t maxPadding{22};
  const size_t padding{GetParameterPadding()};
  bool useMaxPadding{padding + extraPadding > maxPadding};

  for (size_t i{0}; i < lines.size(); ++i) {
    if (useMaxPadding) {
      if (i == 0) fmt::print("{}\n", name);
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

std::string Help::GetCommandName(const Ref<CLI::Instruction>& instruction) {
  LITR_PROFILE_FUNCTION();

  size_t offset{0};
  std::vector<std::string> scope{};

  while (offset < instruction->Count()) {
    const auto code{static_cast<CLI::Instruction::Code>(instruction->Read(offset++))};
    switch (code) {
      case CLI::Instruction::Code::BEGIN_SCOPE: {
        const std::string value{instruction->ReadConstant(instruction->Read(offset))};
        scope.push_back(value);
        offset++;
        break;
      }
      case CLI::Instruction::Code::CLEAR: {
        scope.pop_back();
        break;
      }
      case CLI::Instruction::Code::DEFINE: {
        const std::string value{instruction->ReadConstant(instruction->Read(offset))};
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
  LITR_PROFILE_FUNCTION();

  Config::Query::Parameters params{m_Query.GetParameters(name)};
  std::string arguments{};

  std::sort(params.begin(), params.end(), SortParameterByRequired);

  for (auto&& param : params) {
    switch (param->Type) {
      case Config::Parameter::Type::STRING:
      case Config::Parameter::Type::ARRAY: {
        const bool isOptional{!param->Default.empty()};
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

size_t Help::GetCommandPadding() const {
  LITR_PROFILE_FUNCTION();

  const Config::Query::Commands commands{
      m_CommandName.empty() ? m_Query.GetCommands() : m_Query.GetCommands(m_CommandName)};
  return GetCommandPadding(commands);
}

size_t Help::GetCommandPadding(const Config::Query::Commands& commands) const {
  LITR_PROFILE_FUNCTION();

  size_t padding{0};

  for (auto&& command : commands) {
    const size_t commandLength{command->Name.length()};

    if (padding < commandLength) {
      padding = commandLength;
    }

    // Add 2 for child command padding alignment. It's a hack, I know ¯\_(ツ)_/¯
    const size_t childPadding{GetCommandPadding(command->ChildCommands) + 2};

    if (padding < childPadding) {
      padding = childPadding;
    }
  }

  return padding;
}

size_t Help::GetParameterPadding() const {
  LITR_PROFILE_FUNCTION();

  const Config::Query::Parameters params{
      m_CommandName.empty() ? m_Query.GetParameters() : m_Query.GetParameters(m_CommandName)};
  size_t padding{0};

  constexpr size_t minPadding{7};  // Min padding based on text "version"
  constexpr size_t paddingLeft{4};  // Includes space for short parameter
  constexpr size_t paddingRight{2};

  for (auto&& param : params) {
    const std::string argument{param->TypeArguments.empty() ? "" : m_ArgumentPlaceholder};
    size_t paramLength{(param->Name.length() + argument.length())};

    if (padding < paramLength) {
      padding = paramLength;
    }
  }

  if (padding < minPadding) padding = minPadding;

  return paddingLeft + padding + paddingRight;
}

bool Help::SortParameterByRequired(const Ref<Config::Parameter>& p1, const Ref<Config::Parameter>& p2) {
  LITR_PROFILE_FUNCTION();

  auto RankParameterRequired{[](const Ref<Config::Parameter>& param) -> int {
    switch (param->Type) {
      case Config::Parameter::Type::STRING:
      case Config::Parameter::Type::ARRAY: {
        const bool isRequired{param->Default.empty()};
        return isRequired ? 1 : 2;
      }
      case Config::Parameter::Type::BOOLEAN: {
        return 2;
      }
    }

    return -1;
  }};

  return RankParameterRequired(p1) < RankParameterRequired(p2);
}

}  // namespace Litr::Hook
