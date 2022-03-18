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

    litr::config::ParameterBuilder builder{file, data, "test"};
    litr::config::Parameter builderResult{*builder.get_result()};
    litr::config::Parameter compare{"test"};

    CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
    CHECK_EQ(sizeof(builderResult), sizeof(compare));
    litr::error::Handler::flush();
  }

  TEST_CASE("ParameterBuilder::add_description") {
    SUBCASE("Emits an error if add_description called without description field") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_description();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message,
          R"(You're missing the "description" field.)");
      litr::error::Handler::flush();
    }

    SUBCASE("Emits an error if description is not a string") {
      const auto [file, data] = CreateTOMLMock("test", "description = 42");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_description();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message,
          R"(The "description" can only be a string.)");
      litr::error::Handler::flush();
    }

    SUBCASE("Extracts a description from toml data") {
      const auto [file, data] = CreateTOMLMock("test", R"(description = "Text")");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_description();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->description, "Text");
      litr::error::Handler::flush();
    }

    SUBCASE("Applies a description directly from a string") {
      const auto [file, data] = CreateTOMLMock("test", "");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_description("Text");

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->description, "Text");
      litr::error::Handler::flush();
    }
  }

  TEST_CASE("ParameterBuilder::add_shortcut") {
    SUBCASE("Does nothing if no shortcut is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_shortcut();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK(builder.get_result()->shortcut.empty());
      litr::error::Handler::flush();
    }

    SUBCASE("Emits an error if shortcut is not a string") {
      const auto [file, data] = CreateTOMLMock("test", "shortcut = 42");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_shortcut();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(
          litr::error::Handler::get_errors()[0].message, R"(A "shortcut" can only be a string.)");
      litr::error::Handler::flush();
    }

    SUBCASE("Emits an error if shortcut is reserved word 'help'") {
      const auto [file, data] = CreateTOMLMock("test", R"(shortcut = "help")");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_shortcut();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message,
          R"(The shortcut name "help" is reserved by Litr.)");
      litr::error::Handler::flush();
    }

    SUBCASE("Emits an error if shortcut is reserved word 'h'") {
      const auto [file, data] = CreateTOMLMock("test", R"(shortcut = "h")");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_shortcut();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message,
          R"(The shortcut name "h" is reserved by Litr.)");
      litr::error::Handler::flush();
    }

    SUBCASE("Extracts the shortcut from toml data") {
      const auto [file, data] = CreateTOMLMock("test", R"(shortcut = "t")");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_shortcut();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->shortcut, "t");
      litr::error::Handler::flush();
    }

    SUBCASE("Emits an error if shortcut is already defined") {
      const auto [file, data] = CreateTOMLMock("test", R"(shortcut = "x")");
      auto param{std::make_shared<litr::config::Parameter>("something")};
      param->shortcut = "x";
      std::vector<std::shared_ptr<litr::config::Parameter>> params{{param}};

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_shortcut(params);

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message,
          R"(The shortcut name "x" is already used for parameter "something".)");
      litr::error::Handler::flush();
    }
  }

  TEST_CASE("ParameterBuilder::add_type") {
    SUBCASE("Does nothing if no type is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->type, litr::config::Parameter::Type::STRING);
      litr::error::Handler::flush();
    }

    SUBCASE("Emits an error if string is set with an unknown option") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = "unknown")");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message,
          R"(The "type" option as string can only be "string" or "boolean". Provided value "unknown" is not known.)");
      litr::error::Handler::flush();
    }

    SUBCASE("Sets the type successfully to String if options is string") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = "string")");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->type, litr::config::Parameter::Type::STRING);
      litr::error::Handler::flush();
    }

    SUBCASE("Sets the type to Array on empty arrays") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = [])");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->type, litr::config::Parameter::Type::ARRAY);
      litr::error::Handler::flush();
    }

    SUBCASE("Emits an error if a non string value is contained in the type array") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = ["1", 2, "3"])");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message,
          R"(The options provided in "type" are not all strings.)");
      litr::error::Handler::flush();
    }

    SUBCASE("Sets the type to Array with and populates TypeArguments") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = ["test", "debug"])");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();
      std::shared_ptr<litr::config::Parameter> result{builder.get_result()};

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(result->type, litr::config::Parameter::Type::ARRAY);
      CHECK_EQ(result->type_arguments[0], "test");
      CHECK_EQ(result->type_arguments[1], "debug");
      litr::error::Handler::flush();
    }

    SUBCASE("Emits an error if type is neither a string nor an array") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = 32)");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message,
          R"(A "type" can only be "string" or an array of options as strings.)");
      litr::error::Handler::flush();
    }
  }

  TEST_CASE("ParameterBuilder::add_default") {
    SUBCASE("Does nothing if default is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_default();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK(builder.get_result()->default_value.empty());
      litr::error::Handler::flush();
    }

    SUBCASE("Emits an error if default is not a string") {
      const auto [file, data] = CreateTOMLMock("test", R"(default = 1)");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_default();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message,
          R"(The field "default" needs to be a string.)");
      litr::error::Handler::flush();
    }

    SUBCASE("Emits an error if default value is not found inside type array") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = ["Not default"]
default = "Default")");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();
      builder.add_default();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message,
          R"(Cannot find default value "Default" inside "type" list defined in line 1.)");
      litr::error::Handler::flush();
    }

    SUBCASE("Sets default if defined as string") {
      const auto [file, data] = CreateTOMLMock("test", R"(default = "something")");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_default();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->default_value, "something");
      litr::error::Handler::flush();
    }

    SUBCASE("Sets default if defined as string and type array contains value") {
      const auto [file, data] = CreateTOMLMock("test", R"(type = ["something"]
default = "something")");

      litr::config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();
      builder.add_default();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->default_value, "something");
      litr::error::Handler::flush();
    }
  }
}
