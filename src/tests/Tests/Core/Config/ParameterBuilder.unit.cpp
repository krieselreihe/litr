#include <doctest/doctest.h>

#include "Helpers/TOML.hpp"
#include "Core/Errors/ErrorHandler.hpp"
#include "Core/Config/ParameterBuilder.hpp"

TEST_SUITE("ParameterBuilder") {
  TEST_CASE("Initiates a Parameter on construction") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto [file, data] = CreateTOMLMock("test", "");

    Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
    Litr::Ref<Litr::Config::Parameter> builderResult{builder.GetResult()};
    Litr::Config::Parameter compare{"test"};

    CHECK(errorHandler->GetErrors().size() == 0);
    CHECK(sizeof(*builderResult) == sizeof(compare));
  }

  TEST_CASE("ParameterBuilder::AddDescription") {
    SUBCASE("Emits an error if AddDescription called without description field") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddDescription();

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == R"(You're missing the "description" field.)");
    }

    SUBCASE("Emits an error if description is not a string") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", "description = 42");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddDescription();

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == R"(The "description" can can only be a string.)");
    }

    SUBCASE("Extracts a description from toml data") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(description = "Text")");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddDescription();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Description == "Text");
    }

    SUBCASE("Applies a description directly from a string") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", "");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddDescription("Text");

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Description == "Text");
    }
  }

  TEST_CASE("ParameterBuilder::AddShortcut") {
    SUBCASE("Does nothing if no shortcut is not set") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddShortcut();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Shortcut.empty());
    }

    SUBCASE("Emits an error if shortcut is not a string") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", "shortcut = 42");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddShortcut();

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == R"(A "shortcut" can can only be a string.)");
    }

    SUBCASE("Emits an error if shortcut is reserved word 'help'") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(shortcut = "help")");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddShortcut();

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == R"(The shortcut name "help" is reserved by Litr.)");
    }

    SUBCASE("Emits an error if shortcut is reserved word 'h'") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(shortcut = "h")");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddShortcut();

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == R"(The shortcut name "h" is reserved by Litr.)");
    }

    SUBCASE("Extracts the shortcut from toml data") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(shortcut = "t")");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddShortcut();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Shortcut == "t");
    }
  }

  TEST_CASE("ParameterBuilder::AddType") {
    SUBCASE("Does nothing if no type is not set") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddType();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Type == Litr::Config::Parameter::ParameterType::String);
    }

    SUBCASE("Emits an error if string is set with an unknown option") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(type = "unknown")");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddType();

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == R"(The "type" option as string can only be "string". Provided value "unknown" is not known.)");
    }

    SUBCASE("Sets the type successfully to String if options is string") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(type = "string")");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddType();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Type == Litr::Config::Parameter::ParameterType::String);
    }

    // @todo: Not sure if this should actually be an error.
    // See: https://github.com/krieselreihe/litr/issues/14
    SUBCASE("Sets the type to Array on empty arrays") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(type = [])");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddType();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Type == Litr::Config::Parameter::ParameterType::Array);
    }

    SUBCASE("Emits an error if a non string value is contained in the type array") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(type = ["1", 2, "3"])");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddType();

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == R"(The options provided in "type" are not all strings.)");
    }

    SUBCASE("Sets the type to Array with and populates TypeArguments") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(type = ["test", "debug"])");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddType();
      Litr::Ref<Litr::Config::Parameter> result{builder.GetResult()};

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(result->Type == Litr::Config::Parameter::ParameterType::Array);
      CHECK(result->TypeArguments[0] == "test");
      CHECK(result->TypeArguments[1] == "debug");
    }

    SUBCASE("Emits an error if type is neither a string nor an array") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(type = 32)");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddType();

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == R"(A "type" can can only be "string" or an array of options as strings.)");
    }
  }

  TEST_CASE("ParameterBuilder::AddDefault") {
    SUBCASE("Does nothing if default is not set") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddDefault();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Default.empty());
    }

    SUBCASE("Emits an error if default is not a string") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(default = 1)");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddDefault();

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == R"(The field "default" needs to be a string.)");
    }

    SUBCASE("Emits an error if default value is not found inside type array") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(type = ["Not default"]
default = "Default")");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddType();
      builder.AddDefault();

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == R"(Cannot find default value "Default" inside "type" list defined in line 1.)");
    }

    SUBCASE("Sets default if defined as string") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(default = "something")");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddDefault();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Default == "something");
    }

    SUBCASE("Sets default if defined as string and type array contains value") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(type = ["something"]
default = "something")");

      Litr::Config::ParameterBuilder builder{errorHandler, file, data, "test"};
      builder.AddType();
      builder.AddDefault();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Default == "something");
    }
  }
}
