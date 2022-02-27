/*
 * Copyright (c) 2020 Martin Helmut Fieber <info@martin-fieber.se>
 */

#pragma once

#include <string>
#include <utility>

#include <toml.hpp>

std::pair<toml::table, toml::value> CreateTOMLMock(const std::string& name, const std::string& toml);
