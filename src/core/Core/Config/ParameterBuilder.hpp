#pragma once

#include <string>
#include <toml.hpp>

#include "Core/Base.hpp"
#include "Core/Config/Parameter.hpp"
#include "Core/Errors/Error.hpp"
#include "Core/Errors/ErrorHandler.hpp"

namespace Litr {

class ParameterBuilder {
 public:
  ParameterBuilder(const Ref<ErrorHandler>& errorHandler, const toml::table& file, const toml::value& data, const std::string& name);

  void AddDescription();
  void AddDescription(const std::string& description);
  void AddShortcut();
  void AddDefault();
  void AddType();

  [[nodiscard]] inline Ref<Parameter> GetResult() const { return m_Parameter; }

  [[nodiscard]] static bool IsReservedName(const std::string& name);

 private:
  const Ref<ErrorHandler>& m_ErrorHandler;
  const toml::table& m_File;
  const toml::value& m_Table;
  const Ref<Parameter> m_Parameter;
};

}  // namespace Litr
