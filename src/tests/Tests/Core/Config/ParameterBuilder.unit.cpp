#include <doctest/doctest.h>
#include <toml.hpp>

#include "Core/Config/ParameterBuilder.hpp"

TEST_SUITE("ParameterBuilder") {
  TEST_CASE("Initiates a Parameter on construction") {
    toml::table file{};
    toml::value data{};
    Litr::ParameterBuilder builder{file, data, "test"};

    // @todo
  }
}
