#include "ParameterBuilder.hpp"

namespace Litr::Config {

ParameterBuilder::ParameterBuilder(const Ref<ErrorHandler>& errorHandler, const toml::table& file, const toml::value& data, const std::string& name)
    : m_ErrorHandler(errorHandler), m_File(file), m_Table(data), m_Parameter(CreateRef<Parameter>(name)) {
  LITR_CORE_TRACE("Creating {}", *m_Parameter);
}

void ParameterBuilder::AddDescription() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"description"};

  if (!m_Table.contains(name)) {
    m_ErrorHandler->Push({
      ErrorType::MALFORMED_PARAM,
      R"(You're missing the "description" field.)",
      m_File.at(m_Parameter->Name)
    });
    return;
  }

  const toml::value& description{toml::find(m_Table, name)};

  if (!description.is_string()) {
    m_ErrorHandler->Push({
      ErrorType::MALFORMED_PARAM,
      fmt::format(R"(The "{}" can can only be a string.)", name),
      m_Table.at(name)
    });
    return;
  }

  m_Parameter->Description = description.as_string();
}

void ParameterBuilder::AddDescription(const std::string& description) {
  LITR_PROFILE_FUNCTION();

  m_Parameter->Description = description;
}

void ParameterBuilder::AddShortcut() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"shortcut"};

  if (m_Table.contains(name)) {
    const toml::value& shortcut{toml::find(m_Table, name)};

    if (shortcut.is_string()) {
      const std::string shortcutStr{shortcut.as_string()};

      if (IsReservedName(shortcutStr)) {
        m_ErrorHandler->Push({
          ErrorType::RESERVED_PARAM,
          fmt::format(R"(The shortcut name "{}" is reserved by Litr.)", shortcutStr),
          m_Table.at(name)
        });
        return;
      }

      m_Parameter->Shortcut = shortcutStr;
      return;
    }

    m_ErrorHandler->Push({
      ErrorType::MALFORMED_PARAM,
      fmt::format(R"(A "{}" can can only be a string.)", name),
      m_Table.at(name)
    });
  }
}

void ParameterBuilder::AddType() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"type"};

  if (m_Table.contains(name)) {
    const toml::value& type{toml::find(m_Table, name)};

    if (type.is_string()) {
      if (type.as_string() == "string") {
        m_Parameter->Type = Parameter::ParameterType::String;
      } else {
        m_ErrorHandler->Push({
          ErrorType::UNKNOWN_PARAM_VALUE,
          fmt::format(R"(The "{}" option as string can only be "string". Provided value "{}" is not known.)", name,
                      static_cast<std::string>(type.as_string())),
          m_Table.at(name)
        });
      }
      return;
    }

    if (type.is_array()) {
      m_Parameter->Type = Parameter::ParameterType::Array;
      for (auto&& option : type.as_array()) {
        if (option.is_string()) {
          m_Parameter->TypeArguments.emplace_back(option.as_string());
        } else {
          m_ErrorHandler->Push({
            ErrorType::MALFORMED_PARAM,
            fmt::format(R"(The options provided in "{}" are not all strings.)", name),
            m_Table.at(name)
          });
        }
      }
      return;
    }

    m_ErrorHandler->Push({
      ErrorType::MALFORMED_PARAM,
      fmt::format(R"(A "{}" can can only be "string" or an array of options as strings.)", name),
      m_Table.at(name)
    });
  }
}

void ParameterBuilder::AddDefault() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"default"};

  if (m_Table.contains(name)) {
    const toml::value& def{toml::find(m_Table, name)};

    if (def.is_string()) {
      const std::string defaultValue{def.as_string()};

      // Test if default value present in available options
      if (m_Parameter->Type == Parameter::ParameterType::Array) {
        const std::vector<std::string>& args{m_Parameter->TypeArguments};
        if (std::find(args.begin(), args.end(), defaultValue) == args.end()) {
          m_ErrorHandler->Push({
           ErrorType::MALFORMED_PARAM,
           fmt::format(
                    R"(Cannot find default value "{}" inside "type" list defined in line {}.)",
                    defaultValue,
                    m_Table.at("type").location().line()
                  ),
           m_Table.at(name)
          });
          return;
        }
      }

      m_Parameter->Default = defaultValue;
      return;
    }

    m_ErrorHandler->Push({
      ErrorType::MALFORMED_PARAM,
      fmt::format(R"(The field "{}" needs to be a string.)", name),
      m_Table.at(name)
    });
  }
}

bool ParameterBuilder::IsReservedName(const std::string& name) {
  LITR_PROFILE_FUNCTION();

  const std::array<std::string, 2> reserved{"help", "h"};
  return std::find(reserved.begin(), reserved.end(), name) != reserved.end();
}

}  // namespace Litr::Config
