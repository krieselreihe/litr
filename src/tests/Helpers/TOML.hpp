/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>

#include "Core/Config/TomlFileAdapter.hpp"

struct TomlMock {
  litr::config::TomlFileAdapter::Value context;
  litr::config::TomlFileAdapter::Value data;
};

TomlMock create_toml_mock(const std::string& name, const std::string& toml);
