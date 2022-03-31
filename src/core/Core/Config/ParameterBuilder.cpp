/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "ParameterBuilder.hpp"

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Error/Handler.hpp"
#include "Core/Log.hpp"

namespace Litr::Config {

ParameterBuilder::ParameterBuilder(const TomlFileAdapter::Value& context,
    const TomlFileAdapter::Value& data,
    const std::string& name)
    : m_context(context),
      m_table(data),
      m_parameter(std::make_shared<Parameter>(name)) {
  LITR_CORE_TRACE("Creating {}", *m_parameter);
}

void ParameterBuilder::add_description() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"description"};

  if (!m_table.contains(name)) {
    Error::Handler::push(Error::MalformedParamError(
        R"(You're missing the "description" field.)", m_context.at(m_parameter->name)));
    return;
  }

  const TomlFileAdapter::Value& description{m_file.find(m_table, name)};

  if (!description.is_string()) {
    Error::Handler::push(Error::MalformedParamError(
        fmt::format(R"(The "{}" can only be a string.)", name), m_table.at(name)));
    return;
  }

  m_parameter->description = description.as_string();
}

void ParameterBuilder::add_description(const std::string& description) {
  LITR_PROFILE_FUNCTION();

  m_parameter->description = description;
}

void ParameterBuilder::add_shortcut() {
  add_shortcut({});
}

void ParameterBuilder::add_shortcut(const std::vector<std::shared_ptr<Parameter>>& params) {
  LITR_PROFILE_FUNCTION();

  const std::string name{"shortcut"};

  if (m_table.contains(name)) {
    const TomlFileAdapter::Value& shortcut{m_file.find(m_table, name)};

    if (shortcut.is_string()) {
      const std::string shortcut_str{shortcut.as_string()};

      if (is_reserved_name(shortcut_str)) {
        Error::Handler::push(Error::ReservedParamError(
            fmt::format(R"(The shortcut name "{}" is reserved by Litr.)", shortcut_str),
            m_table.at(name)));
        return;
      }

      for (auto&& param : params) {
        if (param->shortcut == shortcut_str) {
          Error::Handler::push(Error::ValueAlreadyInUseError(
              fmt::format(R"(The shortcut name "{}" is already used for parameter "{}".)",
                  shortcut_str,
                  param->name),
              m_table.at(name)));
          return;
        }
      }

      m_parameter->shortcut = shortcut_str;
      return;
    }

    Error::Handler::push(Error::MalformedParamError(
        fmt::format(R"(A "{}" can only be a string.)", name), m_table.at(name)));
  }
}

void ParameterBuilder::add_type() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"type"};

  if (m_table.contains(name)) {
    const TomlFileAdapter::Value& type{m_file.find(m_table, name)};

    if (type.is_string()) {
      if (type.as_string() == "string") {
        m_parameter->type = Parameter::Type::STRING;
      } else if (type.as_string() == "boolean") {
        m_parameter->type = Parameter::Type::BOOLEAN;
      } else {
        Error::Handler::push(Error::UnknownParamValueError(
            fmt::format(
                R"(The "{}" option as string can only be "string" or "boolean". Provided value "{}" is not known.)",
                name,
                // @todo: Looking at this I'm confused why I have to do this. Needs some
                // investigation.
                static_cast<std::string>(type.as_string())),
            m_table.at(name)));
      }
      return;
    }

    if (type.is_array()) {
      m_parameter->type = Parameter::Type::ARRAY;
      for (auto&& option : type.as_array()) {
        if (option.is_string()) {
          m_parameter->type_arguments.emplace_back(option.as_string());
        } else {
          Error::Handler::push(Error::MalformedParamError(
              fmt::format(R"(The options provided in "{}" are not all strings.)", name),
              m_table.at(name)));
        }
      }
      return;
    }

    Error::Handler::push(Error::MalformedParamError(
        fmt::format(R"(A "{}" can only be "string" or an array of options as strings.)", name),
        m_table.at(name)));
  }
}

void ParameterBuilder::add_default() {
  LITR_PROFILE_FUNCTION();

  const std::string name{"default"};

  if (m_table.contains(name)) {
    const TomlFileAdapter::Value& def{m_file.find(m_table, name)};

    if (def.is_string()) {
      const std::string default_value{def.as_string()};

      // Test if default value present in available options
      if (m_parameter->type == Parameter::Type::ARRAY) {
        const std::vector<std::string>& args{m_parameter->type_arguments};
        if (std::find(args.begin(), args.end(), default_value) == args.end()) {
          Error::Handler::push(Error::MalformedParamError(
              fmt::format(
                  R"(Cannot find default value "{}" inside "type" list defined in line {}.)",
                  default_value,
                  m_table.at("type").location().line()),
              m_table.at(name)));
          return;
        }
      }

      m_parameter->default_value = default_value;
      return;
    }

    Error::Handler::push(Error::MalformedParamError(
        fmt::format(R"(The field "{}" needs to be a string.)", name), m_table.at(name)));
  }
}

bool ParameterBuilder::is_reserved_name(const std::string& name) {
  LITR_PROFILE_FUNCTION();

  // @todo: Could help and version be closer to the hooks?
  const std::array<std::string, 6> reserved{
      // Those are reserved to not collide with the built-in help
      "help",
      "h",
      // Those are reserved to not collide with the built-in version
      "version",
      "v",
      // Those are reserved for script functionality
      "or",
      "and"};
  return std::find(reserved.begin(), reserved.end(), name) != reserved.end();
}

}  // namespace Litr::Config
