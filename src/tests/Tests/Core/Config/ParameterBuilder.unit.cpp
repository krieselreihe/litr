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
    SUBCASE("Does nothing if no shortcut is not set") {
      auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddShortcut();

      CHECK(builder.GetErrors().size() == 0);
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

  TEST_CASE("ParameterBuilder::AddType") {
    SUBCASE("Does nothing if no type is not set") {
      auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK(builder.GetErrors().size() == 0);
      CHECK(builder.GetResult()->Type == Litr::Parameter::ParameterType::String);
    }

    SUBCASE("Emits an error if string is set with an unknown option") {
      auto [file, data] = CreateTOMLMock("test", R"(type = "unknown")");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK(builder.GetErrors().size() == 1);
      CHECK(builder.GetErrors()[0].Message == R"(The "type" option as string can only be "string". Provided value "unknown" is not known.)");
    }

    SUBCASE("Sets the type successfully to String if options is string") {
      auto [file, data] = CreateTOMLMock("test", R"(type = "string")");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK(builder.GetErrors().size() == 0);
      CHECK(builder.GetResult()->Type == Litr::Parameter::ParameterType::String);
    }

    // @todo: Not sure if this should actually be an error.
    // See: https://github.com/krieselreihe/litr/issues/14
    SUBCASE("Sets the type to Array on empty arrays") {
      auto [file, data] = CreateTOMLMock("test", R"(type = [])");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK(builder.GetErrors().size() == 0);
      CHECK(builder.GetResult()->Type == Litr::Parameter::ParameterType::Array);
    }

    SUBCASE("Emits an error if a non string value is contained in the type array") {
      auto [file, data] = CreateTOMLMock("test", R"(type = ["1", 2, "3"])");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK(builder.GetErrors().size() == 1);
      CHECK(builder.GetErrors()[0].Message == R"(The options provided in "type" are not all strings.)");
    }

    SUBCASE("Sets the type to Array with and populates TypeArguments") {
      auto [file, data] = CreateTOMLMock("test", R"(type = ["test", "debug"])");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddType();
      Litr::Ref<Litr::Parameter> result{builder.GetResult()};

      CHECK(builder.GetErrors().size() == 0);
      CHECK(result->Type == Litr::Parameter::ParameterType::Array);
      CHECK(result->TypeArguments[0] == "test");
      CHECK(result->TypeArguments[1] == "debug");
    }

    SUBCASE("Emits an error if type is neither a string nor an array") {
      auto [file, data] = CreateTOMLMock("test", R"(type = 32)");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK(builder.GetErrors().size() == 1);
      CHECK(builder.GetErrors()[0].Message == R"(A "type" can can only be "string" or an array of options as strings.)");
    }
  }

  TEST_CASE("ParameterBuilder::AddDefault") {
    SUBCASE("Does nothing if default is not set") {
      auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddDefault();

      CHECK(builder.GetErrors().size() == 0);
      CHECK(builder.GetResult()->Default.empty());
    }

    SUBCASE("Emits an error if default is not a string") {
      auto [file, data] = CreateTOMLMock("test", R"(default = 1)");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddDefault();

      CHECK(builder.GetErrors().size() == 1);
      CHECK(builder.GetErrors()[0].Message == R"(The field "default" needs to be a string.)");
    }

    SUBCASE("Sets default if defined as string") {
      auto [file, data] = CreateTOMLMock("test", R"(default = "something")");

      Litr::ParameterBuilder builder{file, data, "test"};
      builder.AddDefault();

      CHECK(builder.GetErrors().size() == 0);
      CHECK(builder.GetResult()->Default == "something");
    }
  }
}
