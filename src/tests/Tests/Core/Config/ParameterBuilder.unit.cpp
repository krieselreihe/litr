/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/Config/ParameterBuilder.hpp"

#include <doctest/doctest.h>

#include "Core/Error/Handler.hpp"
#include "Helpers/TOML.hpp"

TEST_SUITE("ParameterBuilder") {
  TEST_CASE("Initiates a Parameter on construction") {
    const auto [file, data] = create_toml_mock("test", "");

    Litr::Config::ParameterBuilder builder{file, data, "test"};
    Litr::Config::Parameter builder_result{*builder.get_result()};
    Litr::Config::Parameter compare{"test"};

    CHECK_EQ(Litr::Error::Handler::get_errors().size(), 0);
    CHECK_EQ(sizeof(builder_result), sizeof(compare));
    Litr::Error::Handler::flush();
  }

  TEST_CASE("ParameterBuilder::add_description") {
    SUBCASE("Emits an error if add_description called without description field") {
      const auto [file, data] = create_toml_mock("test", R"(key = "value")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_description();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::get_errors()[0].message,
          R"(You're missing the "description" field.)");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits an error if description is not a string") {
      const auto [file, data] = create_toml_mock("test", "description = 42");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_description();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::get_errors()[0].message,
          R"(The "description" can only be a string.)");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Extracts a description from toml data") {
      const auto [file, data] = create_toml_mock("test", R"(description = "Text")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_description();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->description, "Text");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Applies a description directly from a string") {
      const auto [file, data] = create_toml_mock("test", "");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_description("Text");

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->description, "Text");
      Litr::Error::Handler::flush();
    }
  }

  TEST_CASE("ParameterBuilder::add_shortcut") {
    SUBCASE("Does nothing if no shortcut is not set") {
      const auto [file, data] = create_toml_mock("test", R"(key = "value")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_shortcut();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 0);
      CHECK(builder.get_result()->shortcut.empty());
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits an error if shortcut is not a string") {
      const auto [file, data] = create_toml_mock("test", "shortcut = 42");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_shortcut();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 1);
      CHECK_EQ(
          Litr::Error::Handler::get_errors()[0].message, R"(A "shortcut" can only be a string.)");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits an error if shortcut is reserved word 'help'") {
      const auto [file, data] = create_toml_mock("test", R"(shortcut = "help")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_shortcut();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::get_errors()[0].message,
          R"(The shortcut name "help" is reserved by Litr.)");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits an error if shortcut is reserved word 'h'") {
      const auto [file, data] = create_toml_mock("test", R"(shortcut = "h")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_shortcut();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::get_errors()[0].message,
          R"(The shortcut name "h" is reserved by Litr.)");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Extracts the shortcut from toml data") {
      const auto [file, data] = create_toml_mock("test", R"(shortcut = "t")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_shortcut();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->shortcut, "t");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits an error if shortcut is already defined") {
      const auto [file, data] = create_toml_mock("test", R"(shortcut = "x")");
      auto param{std::make_shared<Litr::Config::Parameter>("something")};
      param->shortcut = "x";
      std::vector<std::shared_ptr<Litr::Config::Parameter>> params{{param}};

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_shortcut(params);

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::get_errors()[0].message,
          R"(The shortcut name "x" is already used for parameter "something".)");
      Litr::Error::Handler::flush();
    }
  }

  TEST_CASE("ParameterBuilder::add_type") {
    SUBCASE("Does nothing if no type is not set") {
      const auto [file, data] = create_toml_mock("test", R"(key = "value")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->type, Litr::Config::Parameter::Type::STRING);
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits an error if string is set with an unknown option") {
      const auto [file, data] = create_toml_mock("test", R"(type = "unknown")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::get_errors()[0].message,
          R"(The "type" option as string can only be "string" or "boolean". Provided value "unknown" is not known.)");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Sets the type successfully to String if options is string") {
      const auto [file, data] = create_toml_mock("test", R"(type = "string")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->type, Litr::Config::Parameter::Type::STRING);
      Litr::Error::Handler::flush();
    }

    SUBCASE("Sets the type to Array on empty arrays") {
      const auto [file, data] = create_toml_mock("test", R"(type = [])");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->type, Litr::Config::Parameter::Type::ARRAY);
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits an error if a non string value is contained in the type array") {
      const auto [file, data] = create_toml_mock("test", R"(type = ["1", 2, "3"])");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::get_errors()[0].message,
          R"(The options provided in "type" are not all strings.)");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Sets the type to Array with and populates TypeArguments") {
      const auto [file, data] = create_toml_mock("test", R"(type = ["test", "debug"])");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();
      std::shared_ptr<Litr::Config::Parameter> result{builder.get_result()};

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 0);
      CHECK_EQ(result->type, Litr::Config::Parameter::Type::ARRAY);
      CHECK_EQ(result->type_arguments[0], "test");
      CHECK_EQ(result->type_arguments[1], "debug");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits an error if type is neither a string nor an array") {
      const auto [file, data] = create_toml_mock("test", R"(type = 32)");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::get_errors()[0].message,
          R"(A "type" can only be "string" or an array of options as strings.)");
      Litr::Error::Handler::flush();
    }
  }

  TEST_CASE("ParameterBuilder::add_default") {
    SUBCASE("Does nothing if default is not set") {
      const auto [file, data] = create_toml_mock("test", R"(key = "value")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_default();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 0);
      CHECK(builder.get_result()->default_value.empty());
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits an error if default is not a string") {
      const auto [file, data] = create_toml_mock("test", R"(default = 1)");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_default();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::get_errors()[0].message,
          R"(The field "default" needs to be a string.)");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits an error if default value is not found inside type array") {
      const auto [file, data] = create_toml_mock("test", R"(type = ["Not default"]
default = "Default")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();
      builder.add_default();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::get_errors()[0].message,
          R"(Cannot find default value "Default" inside "type" list defined in line 1.)");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Sets default if defined as string") {
      const auto [file, data] = create_toml_mock("test", R"(default = "something")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_default();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->default_value, "something");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Sets default if defined as string and type array contains value") {
      const auto [file, data] = create_toml_mock("test", R"(type = ["something"]
default = "something")");

      Litr::Config::ParameterBuilder builder{file, data, "test"};
      builder.add_type();
      builder.add_default();

      CHECK_EQ(Litr::Error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->default_value, "something");
      Litr::Error::Handler::flush();
    }
  }
}
