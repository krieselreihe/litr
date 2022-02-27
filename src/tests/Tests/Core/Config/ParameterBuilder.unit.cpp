/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/Config/ParameterBuilder.hpp"

#include <doctest/doctest.h>

#include "Core/Error/Handler.hpp"
#include "Helpers/TOML.hpp"

TEST_SUITE("ParameterBuilder") {
  TEST_CASE("Initiates a Parameter on construction") {
    const auto [file, data] = CreateTOMLMock("test", "");

    litr::Config::ParameterBuilder builder{file, data, "test"};
    litr::Ref<litr::Config::Parameter> builderResult{builder.GetResult()};
    litr::Config::Parameter compare{"test"};

    CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
    CHECK_EQ(sizeof(*builderResult), sizeof(compare));
    litr::Error::Handler::Flush();
  }

  TEST_CASE("ParameterBuilder::AddDescription") {
    SUBCASE("Emits an error if AddDescription called without description field") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(You're missing the "description" field.)");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if description is not a string") {
      const auto [file, data] = CreateTOMLMock("test", "description = 42");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(The "description" can only be a string.)");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Extracts a description from toml data") {
      const auto [file, data] = CreateTOMLMock("test", R"(description = "Text")");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Description, "Text");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Applies a description directly from a string") {
      const auto [file, data] = CreateTOMLMock("test", "");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddDescription("Text");

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Description, "Text");
      litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("ParameterBuilder::AddShortcut") {
    SUBCASE("Does nothing if no shortcut is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddShortcut();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK(builder.GetResult()->Shortcut.empty());
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if shortcut is not a string") {
      const auto [file, data] = CreateTOMLMock("test", "shortcut = 42");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddShortcut();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(A "shortcut" can only be a string.)");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if shortcut is reserved word 'help'") {
      const auto [file, data] = CreateTOMLMock("test", R"(shortcut = "help")");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddShortcut();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(The shortcut name "help" is reserved by Litr.)");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if shortcut is reserved word 'h'") {
      const auto [file, data] = CreateTOMLMock("test", R"(shortcut = "h")");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddShortcut();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(The shortcut name "h" is reserved by Litr.)");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Extracts the shortcut from toml data") {
      const auto [file, data] = CreateTOMLMock("test", R"(shortcut = "t")");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddShortcut();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Shortcut, "t");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if shortcut is already defined") {
      const auto [file, data] = CreateTOMLMock("test", R"(shortcut = "x")");
      auto param{litr::CreateRef<litr::Config::Parameter>("something")};
      param->Shortcut = "x";
      std::vector<litr::Ref<litr::Config::Parameter>> params{{param}};

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddShortcut(params);

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(The shortcut name "x" is already used for parameter "something".)");
      litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("ParameterBuilder::AddType") {
    SUBCASE("Does nothing if no type is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Type, litr::Config::Parameter::Type::STRING);
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if string is set with an unknown option") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = "unknown")");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(The "type" option as string can only be "string" or "boolean". Provided value "unknown" is not known.)");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Sets the type successfully to String if options is string") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = "string")");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Type, litr::Config::Parameter::Type::STRING);
      litr::Error::Handler::Flush();
    }

    SUBCASE("Sets the type to Array on empty arrays") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = [])");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Type, litr::Config::Parameter::Type::ARRAY);
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if a non string value is contained in the type array") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = ["1", 2, "3"])");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(The options provided in "type" are not all strings.)");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Sets the type to Array with and populates TypeArguments") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = ["test", "debug"])");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();
      litr::Ref<litr::Config::Parameter> result{builder.GetResult()};

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(result->Type, litr::Config::Parameter::Type::ARRAY);
      CHECK_EQ(result->TypeArguments[0], "test");
      CHECK_EQ(result->TypeArguments[1], "debug");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if type is neither a string nor an array") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = 32)");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(A "type" can only be "string" or an array of options as strings.)");
      litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("ParameterBuilder::AddDefault") {
    SUBCASE("Does nothing if default is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddDefault();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK(builder.GetResult()->Default.empty());
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if default is not a string") {
      const auto [file, data] = CreateTOMLMock("test", R"(default = 1)");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddDefault();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(The field "default" needs to be a string.)");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if default value is not found inside type array") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = ["Not default"]
default = "Default")");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();
      builder.AddDefault();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(Cannot find default value "Default" inside "type" list defined in line 1.)");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Sets default if defined as string") {
      const auto [file, data] = CreateTOMLMock("test", R"(default = "something")");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddDefault();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Default, "something");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Sets default if defined as string and type array contains value") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = ["something"]
default = "something")");

      litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();
      builder.AddDefault();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Default, "something");
      litr::Error::Handler::Flush();
    }
  }
}
