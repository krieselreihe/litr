/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Core/Config/Parameter.hpp"
#include "Core/Config/TomlFileAdapter.hpp"

namespace Litr::Config {

class ParameterBuilder {
 public:
  ParameterBuilder(const TomlFileAdapter::Value& context,
      const TomlFileAdapter::Value& data,
      const std::string& name);

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
  const TomlFileAdapter::Value& m_context;
  const TomlFileAdapter::Value& m_table;
  const std::shared_ptr<Parameter> m_parameter;
  const TomlFileAdapter m_file{};
};

}  // namespace Litr::Config
