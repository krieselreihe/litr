#include "Value.hpp"

namespace Litr::Toms {

Value::Value(const std::string& path) {
  try {
    m_Data = toml::parse<toml::discard_comments, tsl::ordered_map>(path);
  } catch (const toml::syntax_error& err) {
    const toml::source_location& location{err.location()};
    m_HasError = true;
    m_Line = location.line();
    m_Column = location.column();
    m_Content = location.line_str();
  }
}

Value::Value(const toml::value& data)
    : m_Data(data),
      m_Line(data.location().line()),
      m_Column(data.location().column()),
      m_Content(data.location().line_str()) {
}

Value Value::Find(const std::string& key) const {
  return Value(toml::find<toml::table, toml::discard_comments, tsl::ordered_map>(m_Data, key));
}

bool Value::Contains(const std::string& key) const {
  return m_Data.contains(key);
}

bool Value::IsTable() const {
  return m_Data.is_table();
}

bool Value::IsString() const {
  return m_Data.is_string();
}

bool Value::IsArray() const {
  return m_Data.is_array();
}

Value Value::AsTable() const {
  return Value(m_Data.as_table());
}

std::string Value::AsString() const {
  return m_Data.as_string();
}

std::vector<Value> Value::AsArray() const {
  std::vector<Value> target{};
  std::copy(m_Data.as_array().begin(), m_Data.as_array().end(), std::back_inserter(target));
  return target;
}

Value Value::At(size_t key) const {
  return Value(m_Data.at(key));
}

Value Value::At(const std::string& key) const {
  return Value(m_Data.at(key));
}

}  // namespace Litr::Toms
