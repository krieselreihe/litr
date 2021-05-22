#pragma once

#include <string>
#include <toml.hpp>

#include "Core/Base.hpp"
#include "Core/Config/Parameter.hpp"

namespace Litr::Config {

class ParameterBuilder {
 public:
  ParameterBuilder(const toml::table& file, const toml::value& data, const std::string& name);

  void AddDescription();
  void AddDescription(const std::string& description);
  void AddShortcut();
  void AddShortcut(const std::vector<Ref<Parameter>>& params);
  void AddDefault();
  void AddType();

  [[nodiscard]] inline Ref<Parameter> GetResult() const { return m_Parameter; }

  [[nodiscard]] static bool IsReservedName(const std::string& name);

 private:
  const toml::table& m_File;
  const toml::value& m_Table;
  const Ref<Parameter> m_Parameter;
};

}  // namespace Litr::Config
