/*
 * Copyright (c) 2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "TomlFileAdapter.hpp"

#include "Core/Debug/Instrumentor.hpp"
#include "Core/Error/Handler.hpp"

namespace litr::config {

TomlFileAdapter::Value TomlFileAdapter::parse(const Path& file_path) const {
  LITR_PROFILE_FUNCTION();

  TomlFileAdapter::Value config{};

  try {
    config = toml::parse<toml::discard_comments, tsl::ordered_map>(file_path.to_string());
  } catch (const toml::syntax_error& err) {
    error::Handler::push(
        error::MalformedFileError("There is a syntax error inside the configuration file.", err));
    return config;
  }

  if (!config.is_table()) {
    error::Handler::push(error::MalformedFileError("Configuration is not a TOML table."));
  }

  return config;
}

const TomlFileAdapter::Value& TomlFileAdapter::find(
    const TomlFileAdapter::Value& value, const std::string& key) const {
  LITR_PROFILE_FUNCTION();

  return toml::find(value, key);
}

}  // namespace litr::config
