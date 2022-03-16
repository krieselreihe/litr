/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <memory>
#include <string>
#include <toml.hpp>
#include <vector>

#include "Core/Config/Parameter.hpp"

namespace litr::config {

class ParameterBuilder {
 public:
  ParameterBuilder(const toml::table& file, const toml::value& data, const std::string& name);

  void add_description();
  void add_description(const std::string& description);
  void add_shortcut();
  void add_shortcut(const std::vector<std::shared_ptr<Parameter>>& params);
  void add_default();
  void add_type();

  [[nodiscard]] inline std::shared_ptr<Parameter> get_result() const {
    return m_parameter;
  }

  [[nodiscard]] static bool is_reserved_name(const std::string& name);

 private:
  const toml::table& m_file;
  const toml::value& m_table;
  const std::shared_ptr<Parameter> m_parameter;
};

}  // namespace litr::config
