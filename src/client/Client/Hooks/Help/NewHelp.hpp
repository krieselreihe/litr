/*
 * Copyright (c) 2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <fmt/color.h>

#include <memory>
#include <string>
#include <vector>

#include "Client/Hooks/Help/HelpStrategy.hpp"
#include "Client/Hooks/Hook.hpp"
#include "Core.hpp"

namespace Litr::Hook {

class NewHelp : public Hook {
 public:
  NewHelp(const std::shared_ptr<Config::Loader>& config,
      const std::shared_ptr<CLI::Instruction>& instruction,
      bool use_color = true);

  [[nodiscard]] std::string read() override;

 private:
  template <class T>
  struct Entry {
    T reference;
    size_t depth;
    std::string value{};
    explicit Entry(const T& ref, size_t dep = 0) : reference(ref), depth(dep) {}
  };

  template <class T>
  using Entries = std::vector<Entry<std::shared_ptr<T>>>;

  [[nodiscard]] std::string welcome_message() const;
  [[nodiscard]] bool same_depth_ahead(size_t command_entry_start_index, size_t target_depth) const;
  [[nodiscard]] std::string line_padding(size_t command_entry_index) const;

  void create_command_entries(const Config::Commands& commands, size_t depth = 0);
  [[nodiscard]] std::string command_arguments(const std::string& name) const;
  static bool sort_parameter_by_required(const std::shared_ptr<Config::Parameter>& param1, const std::shared_ptr<Config::Parameter>& param2);
  void create_parameters_entries(const Config::Parameters& params);
  void define_left_margin();

  template <typename... Args>
  [[nodiscard]] std::string format_with_color(const fmt::text_style& ts, const Args&... args) const;

  std::unique_ptr<HelpStrategy> m_help_strategy;
  const Config::Query m_query;
  bool m_use_color;

  Entries<Config::Command> m_command_entries{};
  Entries<Config::Parameter> m_parameter_entries{};

  static constexpr size_t M_MAX_DESCRIPTION_INSET{22};
  static constexpr size_t M_MAX_LINE_LENGTH{80};

  size_t m_left_side_margin{0};
};

}  // namespace Litr::Hook
