/*
 * Copyright (c) 2020 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>
#include <toml.hpp>
#include <utility>

std::pair<toml::table, toml::value> create_toml_mock(
    const std::string& name, const std::string& toml);
