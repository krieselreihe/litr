#include "ParameterBuilder.hpp"

namespace Litr {

ParameterBuilder::ParameterBuilder(const toml::table& file, const toml::value& data, const std::string& name) : m_File(file), m_Table(data) {
  m_Parameter = CreateRef<Parameter>(name);
  LITR_CORE_TRACE("Creating {}", *m_Parameter);
}

void ParameterBuilder::AddDescription() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"description"};

  if (!m_Table.contains(name)) {
    m_Errors.emplace_back(
        ConfigurationErrorType::MALFORMED_PARAM,
        R"(You're missing the "description" field.)",
        m_File.at(m_Parameter->Name));
    return;
  }

  const toml::value& description{toml::find(m_Table, name)};
  if (!description.is_string()) {
    m_Errors.emplace_back(
        ConfigurationErrorType::MALFORMED_PARAM,
        fmt::format(R"(The "{}" can can only be a string.)", name),
        m_Table.at(name));
    return;
  }

  m_Parameter->Description = description.as_string();
}

void ParameterBuilder::AddDescription(const std::string& description) {
  m_Parameter->Description = description;
}

void ParameterBuilder::AddShortcut() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"shortcut"};

  if (m_Table.contains(name)) {
    const toml::value& shortcut{toml::find(m_Table, name)};
    if (shortcut.is_string()) {
      m_Parameter->Shortcut = shortcut.as_string();
    } else {
      m_Errors.emplace_back(
          ConfigurationErrorType::MALFORMED_PARAM,
          fmt::format(R"(A "{}" can can only be a string.)", name),
          m_Table.at(name));
    }
  }
}

void ParameterBuilder::AddType() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"type"};

  if (m_Table.contains(name)) {
    if (m_Table.is_array()) {
      m_Parameter->Type = Parameter::ParameterType::Array;
      for (auto pa : m_Table.as_array()) {
        m_Parameter->TypeArguments.emplace_back(pa.as_string());
      }
    }
  }
}

void ParameterBuilder::AddDefault() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"default"};

  if (m_Table.contains(name)) {
    const toml::value& def{toml::find(m_Table, name)};
    if (def.is_string()) {
      m_Parameter->Default = def.as_string();
    } else {
      m_Errors.emplace_back(
          ConfigurationErrorType::MALFORMED_PARAM,
          fmt::format(R"(The field "{}" needs to be a string.)", name),
          m_Table.at(name));
    }
  }
}

}  // namespace Litr
