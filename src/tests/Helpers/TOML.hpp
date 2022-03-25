/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>
#include <utility>

#include "Core/Config/TomlFileAdapter.hpp"

std::pair<litr::config::TomlFileAdapter::Table, litr::config::TomlFileAdapter::Value>
create_toml_mock(const std::string& name, const std::string& toml);
