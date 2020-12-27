#pragma once

#include <toml.hpp>

#include <string>

std::pair<toml::table, toml::value> CreateTOMLMock(const std::string& name, const std::string& toml);
