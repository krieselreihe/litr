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

    Litr::Config::ParameterBuilder builder{file, data, "test"};
    Litr::Ref<Litr::Config::Parameter> builderResult{builder.GetResult()};
    Litr::Config::Parameter compare{"test"};

    CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 0);
    CHECK_EQ(sizeof(*builderResult), sizeof(compare));
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("ParameterBuilder::AddDescription") {
    SUBCASE("Emits an error if AddDescription called without description field") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::GetErrors()[0].Message, R"(You're missing the "description" field.)");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if description is not a string") {
      const auto [file, data] = CreateTOMLMock("test", "description = 42");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::GetErrors()[0].Message, R"(The "description" can only be a string.)");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Extracts a description from toml data") {
      const auto [file, data] = CreateTOMLMock("test", R"(description = "Text")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Description, "Text");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Applies a description directly from a string") {
      const auto [file, data] = CreateTOMLMock("test", "");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddDescription("Text");

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Description, "Text");
      Litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("ParameterBuilder::AddShortcut") {
    SUBCASE("Does nothing if no shortcut is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddShortcut();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 0);
      CHECK(builder.GetResult()->Shortcut.empty());
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if shortcut is not a string") {
      const auto [file, data] = CreateTOMLMock("test", "shortcut = 42");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddShortcut();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::GetErrors()[0].Message, R"(A "shortcut" can only be a string.)");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if shortcut is reserved word 'help'") {
      const auto [file, data] = CreateTOMLMock("test", R"(shortcut = "help")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddShortcut();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::GetErrors()[0].Message, R"(The shortcut name "help" is reserved by Litr.)");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if shortcut is reserved word 'h'") {
      const auto [file, data] = CreateTOMLMock("test", R"(shortcut = "h")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddShortcut();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::GetErrors()[0].Message, R"(The shortcut name "h" is reserved by Litr.)");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Extracts the shortcut from toml data") {
      const auto [file, data] = CreateTOMLMock("test", R"(shortcut = "t")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddShortcut();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Shortcut, "t");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if shortcut is already defined") {
      const auto [file, data] = CreateTOMLMock("test", R"(shortcut = "x")");
      auto param{Litr::CreateRef<Litr::Config::Parameter>("something")};
      param->Shortcut = "x";
      std::vector<Litr::Ref<Litr::Config::Parameter>> params{{param}};

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddShortcut(params);

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::GetErrors()[0].Message, R"(The shortcut name "x" is already used for parameter "something".)");
      Litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("ParameterBuilder::AddType") {
    SUBCASE("Does nothing if no type is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Type, Litr::Config::Parameter::Type::STRING);
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if string is set with an unknown option") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = "unknown")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::GetErrors()[0].Message, R"(The "type" option as string can only be "string" or "boolean". Provided value "unknown" is not known.)");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Sets the type successfully to String if options is string") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = "string")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Type, Litr::Config::Parameter::Type::STRING);
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Sets the type to Array on empty arrays") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = [])");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Type, Litr::Config::Parameter::Type::ARRAY);
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if a non string value is contained in the type array") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = ["1", 2, "3"])");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::GetErrors()[0].Message, R"(The options provided in "type" are not all strings.)");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Sets the type to Array with and populates TypeArguments") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = ["test", "debug"])");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();
      Litr::Ref<Litr::Config::Parameter> result{builder.GetResult()};

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(result->Type, Litr::Config::Parameter::Type::ARRAY);
      CHECK_EQ(result->TypeArguments[0], "test");
      CHECK_EQ(result->TypeArguments[1], "debug");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if type is neither a string nor an array") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = 32)");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::GetErrors()[0].Message, R"(A "type" can only be "string" or an array of options as strings.)");
      Litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("ParameterBuilder::AddDefault") {
    SUBCASE("Does nothing if default is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddDefault();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 0);
      CHECK(builder.GetResult()->Default.empty());
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if default is not a string") {
      const auto [file, data] = CreateTOMLMock("test", R"(default = 1)");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddDefault();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::GetErrors()[0].Message, R"(The field "default" needs to be a string.)");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if default value is not found inside type array") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = ["Not default"]
default = "Default")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();
      builder.AddDefault();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::GetErrors()[0].Message, R"(Cannot find default value "Default" inside "type" list defined in line 1.)");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Sets default if defined as string") {
      const auto [file, data] = CreateTOMLMock("test", R"(default = "something")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddDefault();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Default, "something");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Sets default if defined as string and type array contains value") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = ["something"]
default = "something")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.AddType();
      builder.AddDefault();

      CHECK_EQ(Litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Default, "something");
      Litr::Error::Handler::Flush();
    }
  }
}
