#include <doctest/doctest.h>

#include "Helpers/TOML.hpp"
#include "Core/Config/CommandBuilder.hpp"

TEST_SUITE("CommandBuilder") {
  TEST_CASE("Initiates a Command on construction") {
    auto [file, data] = CreateTOMLMock("test", "");

    Litr::CommandBuilder builder{file, data, "test"};
    Litr::Ref<Litr::Command> builderResult{builder.GetResult()};
    Litr::Command compare{"test"};

    CHECK(builder.GetErrors().size() == 0);
    CHECK(sizeof(*builderResult) == sizeof(compare));
  }

  TEST_CASE("CommandBuilder::AddScriptLine") {
    SUBCASE("Can add multiple lines of script to the command") {
      auto [file, data] = CreateTOMLMock("test", "");

      Litr::CommandBuilder builder{file, data, "test"};
      builder.AddScriptLine("first line");
      builder.AddScriptLine("second line");

      auto result{builder.GetResult()};

      CHECK(builder.GetErrors().size() == 0);
      CHECK(result->Script[0] == "first line");
      CHECK(result->Script[1] == "second line");
    }
  }

  TEST_CASE("CommandBuilder::AddScript") {
    SUBCASE("Can override the whole script at once") {
      auto [file, data] = CreateTOMLMock("test", "");

      Litr::CommandBuilder builder{file, data, "test"};
      const std::vector script{"first line", "second line"};
      builder.AddScript(script);

      auto result{builder.GetResult()};

      CHECK(builder.GetErrors().size() == 0);
      CHECK(result->Script[0] == "first line");
      CHECK(result->Script[1] == "second line");
    }

    SUBCASE("Emits and error if script data is not of type string") {
      auto [file, data] = CreateTOMLMock("test", "scripts = [1]");

      Litr::CommandBuilder builder{file, data, "test"};
      builder.AddScript(toml::find(data, "scripts"));

      CHECK(builder.GetErrors().size() == 1);
      CHECK(builder.GetErrors()[0].Message == "A command script can be either a string or array of strings.");
    }

    SUBCASE("Emits only one error if script data is not of type string") {
      auto [file, data] = CreateTOMLMock("test", "scripts = [1, 2, 3]");

      Litr::CommandBuilder builder{file, data, "test"};
      builder.AddScript(toml::find(data, "scripts"));

      CHECK(builder.GetErrors().size() == 1);
      CHECK(builder.GetErrors()[0].Message == "A command script can be either a string or array of strings.");
    }

    SUBCASE("Writes scripts from TOML data successfully") {
      auto [file, data] = CreateTOMLMock("test", R"(scripts = ["first line", "second line"])");

      Litr::CommandBuilder builder{file, data, "test"};
      builder.AddScript(toml::find(data, "scripts"));

      auto result{builder.GetResult()};

      CHECK(builder.GetErrors().size() == 0);
      CHECK(result->Script[0] == "first line");
      CHECK(result->Script[1] == "second line");
    }
  }

  TEST_CASE("CommandBuilder::AddDescription") {
    SUBCASE("Does nothing if description is not set") {
      auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::CommandBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK(builder.GetErrors().size() == 0);
      CHECK(builder.GetResult()->Description.empty());
    }

    SUBCASE("Emits and error if description is not a string") {
      auto [file, data] = CreateTOMLMock("test", R"(description = 42)");

      Litr::CommandBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK(builder.GetErrors().size() == 1);
      CHECK(builder.GetErrors()[0].Message == R"(The "description" can can only be a string.)");
    }

    SUBCASE("Extracts the description from toml data") {
      auto [file, data] = CreateTOMLMock("test", R"(description = "Text")");

      Litr::CommandBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK(builder.GetErrors().size() == 0);
      CHECK(builder.GetResult()->Description == "Text");
    }
  }

  TEST_CASE("CommandBuilder::AddExample") {
    SUBCASE("Does nothing if example is not set") {
      auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::CommandBuilder builder{file, data, "test"};
      builder.AddExample();

      CHECK(builder.GetErrors().size() == 0);
      CHECK(builder.GetResult()->Example.empty());
    }

    SUBCASE("Emits and error if example is not a string") {
      auto [file, data] = CreateTOMLMock("test", R"(example = 42)");

      Litr::CommandBuilder builder{file, data, "test"};
      builder.AddExample();

      CHECK(builder.GetErrors().size() == 1);
      CHECK(builder.GetErrors()[0].Message == R"(The "example" can can only be a string.)");
    }

    SUBCASE("Extracts the example from toml data") {
      auto [file, data] = CreateTOMLMock("test", R"(example = "Text")");

      Litr::CommandBuilder builder{file, data, "test"};
      builder.AddExample();

      CHECK(builder.GetErrors().size() == 0);
      CHECK(builder.GetResult()->Example == "Text");
    }
  }
}
