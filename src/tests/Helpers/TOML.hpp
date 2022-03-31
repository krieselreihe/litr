/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>

#include "Core/Config/TomlFileAdapter.hpp"

struct TomlMock {
  Litr::Config::TomlFileAdapter::Value context;
  Litr::Config::TomlFileAdapter::Value data;
};

TomlMock create_toml_mock(const std::string& name, const std::string& toml);
