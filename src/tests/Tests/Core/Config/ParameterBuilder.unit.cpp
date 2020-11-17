#include <doctest/doctest.h>

#include "Helpers/TOML.hpp"
#include "Core/Config/ParameterBuilder.hpp"

TEST_SUITE("ParameterBuilder") {
  TEST_CASE("Initiates a Parameter on construction") {
    auto [file, data] = CreateTOMLMock("test", "");

    Litr::ParameterBuilder builder{file, data, "test"};
    Litr::Ref<Litr::Parameter> builderResult{builder.GetResult()};
    Litr::Parameter compare{"test"};

    CHECK(builder.GetErrors().size() == 0);
    CHECK(sizeof(*builderResult) == sizeof(compare));
  }

  TEST_CASE("ParameterBuilder::AddDescription") {
    SUBCASE("Emits an error if AddDescription called without description field") {
      auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK(builder.GetErrors().size() == 1);
      CHECK(builder.GetErrors()[0].Message == R"(You're missing the "description" field.)");
    }

    SUBCASE("Emits an error if description is not a string") {
      auto [file, data] = CreateTOMLMock("test", "description = 42");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK(builder.GetErrors().size() == 1);
      CHECK(builder.GetErrors()[0].Message == R"(The "description" can can only be a string.)");
    }

    SUBCASE("Extracts a description from toml data") {
      auto [file, data] = CreateTOMLMock("test", R"(description = "Text")");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK(builder.GetErrors().size() == 0);
      CHECK(builder.GetResult()->Description == "Text");
    }

    SUBCASE("Applies a description directly from a string") {
      auto [file, data] = CreateTOMLMock("test", "");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddDescription("Text");

      CHECK(builder.GetErrors().size() == 0);
      CHECK(builder.GetResult()->Description == "Text");
    }
  }

  TEST_CASE("ParameterBuilder::AddShortcut") {
    SUBCASE("Does nothing if no shortcut is set") {
      auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddShortcut();

      CHECK(builder.GetResult()->Shortcut.empty());
    }

    SUBCASE("Emits an error if shortcut is not a string") {
      auto [file, data] = CreateTOMLMock("test", "shortcut = 42");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddShortcut();

      CHECK(builder.GetErrors().size() == 1);
      CHECK(builder.GetErrors()[0].Message == R"(A "shortcut" can can only be a string.)");
    }

    SUBCASE("Extracts the shortcut from toml data") {
      auto [file, data] = CreateTOMLMock("test", R"(shortcut = "t")");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddShortcut();

      CHECK(builder.GetErrors().size() == 0);
      CHECK(builder.GetResult()->Shortcut == "t");
    }
  }
}
