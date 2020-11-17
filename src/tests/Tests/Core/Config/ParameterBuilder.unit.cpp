#include <doctest/doctest.h>
#include <toml.hpp>

#include "Core/Config/ParameterBuilder.hpp"

using namespace toml::literals::toml_literals;

TEST_SUITE("ParameterBuilder") {
  // Base tests

  TEST_CASE("Initiates a Parameter on construction") {
    const toml::table file{};
    const toml::value data{};

    Litr::ParameterBuilder builder{file, data, "test"};
    Litr::Ref<Litr::Parameter> builderResult{builder.GetResult()};
    Litr::Parameter compare{"test"};

    CHECK(builder.GetErrors().size() == 0);
    CHECK(sizeof(*builderResult) == sizeof(compare));
  }

  TEST_CASE("ParameterBuilder::AddDescription") {
    SUBCASE("Emits an error if AddDescription called without description field") {
      const auto file = u8R"(test.key = "value")"_toml;
      const auto data = u8R"(key = "value")"_toml;

      Litr::ParameterBuilder builder{file.as_table(), data, "test"};
      builder.AddDescription();

      CHECK(builder.GetErrors().size() == 1);
      CHECK(builder.GetErrors()[0].Message == R"(You're missing the "description" field.)");
    }

    SUBCASE("Emits an error if description is not a string") {
      const auto file = u8R"(test.description = 42)"_toml;
      const auto data = u8R"(description = 42)"_toml;

      Litr::ParameterBuilder builder{file.as_table(), data, "test"};
      builder.AddDescription();

      CHECK(builder.GetErrors().size() == 1);
      CHECK(builder.GetErrors()[0].Message == R"(The "description" can can only be a string.)");
    }

    SUBCASE("Extracts a description from toml data") {
      const auto file = u8R"(test.description = "Text")"_toml;
      const auto data = u8R"(description = "Text")"_toml;

      Litr::ParameterBuilder builder{file.as_table(), data, "test"};
      builder.AddDescription();

      CHECK(builder.GetErrors().size() == 0);
      CHECK(builder.GetResult()->Description == "Text");
    }

    SUBCASE("Applies a description directly from a string") {
      const toml::table file{};
      const toml::value data{};

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddDescription("Text");

      CHECK(builder.GetErrors().size() == 0);
      CHECK(builder.GetResult()->Description == "Text");
    }
  }
}
