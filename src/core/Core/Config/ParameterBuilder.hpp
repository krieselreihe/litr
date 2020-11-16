#pragma once

#include <string>
#include <toml.hpp>

#include "Core/Base.hpp"
#include "Core/Config/Parameter.hpp"
#include "Core/Errors/ConfigurationError.hpp"

namespace Litr {

class ParameterBuilder {
 public:
  ParameterBuilder(const toml::table& file, const toml::value& data, const std::string& name);

  void AddDescription();
  void AddDescription(const std::string& description);
  void AddShortcut();
  void AddDefault();
  void AddType();

  [[nodiscard]] inline Ref<Parameter> GetResult() const { return m_Parameter; }
  [[nodiscard]] inline std::vector<ConfigurationError> GetErrors() const { return m_Errors; };

 private:
  Ref<Parameter> m_Parameter;

  const toml::table& m_File;
  const toml::value& m_Table;

  std::vector<ConfigurationError> m_Errors{};
};

}  // namespace Litr
