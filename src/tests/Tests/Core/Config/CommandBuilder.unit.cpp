/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/Config/CommandBuilder.hpp"

#include <doctest/doctest.h>

#include "Core/Error/Handler.hpp"
#include "Helpers/TOML.hpp"

TEST_SUITE("Config::CommandBuilder") {
  TEST_CASE("Initiates a Command on construction") {
    const auto [file, data] = CreateTOMLMock("test", "");

    litr::Config::CommandBuilder builder{file, data, "test"};
    litr::Ref<litr::Config::Command> builderResult{builder.GetResult()};
    litr::Config::Command compare{"test"};

    CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
    CHECK_EQ(sizeof(*builderResult), sizeof(compare));
    litr::Error::Handler::Flush();
  }

  TEST_CASE("CommandBuilder::AddScriptLine") {
    SUBCASE("Can add multiple lines of script to the command") {
      const auto [file, data] = CreateTOMLMock("test", "");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddScriptLine("first line");
      builder.AddScriptLine("second line");

      const auto result{builder.GetResult()};

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(result->Script[0], "first line");
      CHECK_EQ(result->Script[1], "second line");
      litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("CommandBuilder::AddScript") {
    SUBCASE("Can override the whole script at once") {
      const auto [file, data] = CreateTOMLMock("test", "");

      litr::Config::CommandBuilder builder{file, data, "test"};
      const std::vector script{"first line", "second line"};
      builder.AddScript(script);

      const auto result{builder.GetResult()};

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(result->Script[0], "first line");
      CHECK_EQ(result->Script[1], "second line");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits and error if script data is not of type string") {
      const auto [file, data] = CreateTOMLMock("test", "scripts = [1]");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddScript(toml::find(data, "scripts"));

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(
          litr::Error::Handler::GetErrors()[0].Message, "A command script can be either a string or array of strings.");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits only one error if script data is not of type string") {
      const auto [file, data] = CreateTOMLMock("test", "scripts = [1, 2, 3]");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddScript(toml::find(data, "scripts"));

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(
          litr::Error::Handler::GetErrors()[0].Message, "A command script can be either a string or array of strings.");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Writes scripts from TOML data successfully") {
      const auto [file, data] = CreateTOMLMock("test", R"(scripts = ["first line", "second line"])");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddScript(toml::find(data, "scripts"));

      const auto result{builder.GetResult()};

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(result->Script[0], "first line");
      CHECK_EQ(result->Script[1], "second line");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Retains locations information") {
      const auto [file, data] = CreateTOMLMock("test", R"(scripts = ["first line", "second line"])");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddScript(toml::find(data, "scripts"));

      const auto result{builder.GetResult()};

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(result->Locations[0].Line, 1);
      CHECK_EQ(result->Locations[0].Column, 12);
      CHECK_EQ(result->Locations[0].LineStr, R"(scripts = ["first line", "second line"])");
      CHECK_EQ(result->Locations[1].Line, 1);
      CHECK_EQ(result->Locations[1].Column, 26);
      CHECK_EQ(result->Locations[1].LineStr, R"(scripts = ["first line", "second line"])");
      litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("CommandBuilder::AddDescription") {
    SUBCASE("Does nothing if description is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK(builder.GetResult()->Description.empty());
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits and error if description is not a string") {
      const auto [file, data] = CreateTOMLMock("test", R"(description = 42)");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(The "description" can only be a string.)");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Extracts the description from toml data") {
      const auto [file, data] = CreateTOMLMock("test", R"(description = "Text")");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Description, "Text");
      litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("CommandBuilder::AddExample") {
    SUBCASE("Does nothing if example is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddExample();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK(builder.GetResult()->Example.empty());
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits and error if example is not a string") {
      const auto [file, data] = CreateTOMLMock("test", R"(example = 42)");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddExample();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(The "example" can only be a string.)");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Extracts the example from toml data") {
      const auto [file, data] = CreateTOMLMock("test", R"(example = "Text")");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddExample();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Example, "Text");
      litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("CommandBuilder::AddDirectory") {
    SUBCASE("Does nothing if dir is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDirectory(litr::Path(""));

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK(builder.GetResult()->Directory.empty());
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if dir is not a string or array of strings") {
      const auto [file, data] = CreateTOMLMock("test", R"(dir = 42)");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDirectory(litr::Path(""));

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(A "dir" can either be a string or array of strings.)");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if dir array does not only contain strings") {
      const auto [file, data] = CreateTOMLMock("test", R"(dir = [1])");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDirectory(litr::Path(""));

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(A "dir" can either be a string or array of strings.)");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits more than one error if dir array does not only contain multiple strings") {
      const auto [file, data] = CreateTOMLMock("test", R"(dir = [1, 2])");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDirectory(litr::Path(""));

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 2);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(A "dir" can either be a string or array of strings.)");
      CHECK_EQ(litr::Error::Handler::GetErrors()[1].Message, R"(A "dir" can either be a string or array of strings.)");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Creates a directory folder from a string") {
      const auto [file, data] = CreateTOMLMock("test", R"(dir = ["folder1"])");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDirectory(litr::Path(""));

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Directory[0], "folder1");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Creates a directory folder from an array of strings") {
      const auto [file, data] = CreateTOMLMock("test", R"(dir = ["folder1", "folder2"])");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDirectory(litr::Path(""));

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Directory[0], "folder1");
      CHECK_EQ(builder.GetResult()->Directory[1], "folder2");
      litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("CommandBuilder::AddOutput") {
    SUBCASE("Does nothing if output is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddOutput();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Output, litr::Config::Command::Output::UNCHANGED);
      litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if output type is not known") {
      const auto [file, data] = CreateTOMLMock("test", R"(output = "unknown")");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddOutput();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 1);
      CHECK_EQ(litr::Error::Handler::GetErrors()[0].Message, R"(The "output" can either be "unchanged" or "silent".)");
      litr::Error::Handler::Flush();
    }

    SUBCASE("Sets the output to silent if the option is provided") {
      const auto [file, data] = CreateTOMLMock("test", R"(output = "silent")");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddOutput();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Output, litr::Config::Command::Output::SILENT);
      litr::Error::Handler::Flush();
    }

    SUBCASE("Sets the output to unchanged if the option is provided") {
      const auto [file, data] = CreateTOMLMock("test", R"(output = "unchanged")");

      litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddOutput();

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builder.GetResult()->Output, litr::Config::Command::Output::UNCHANGED);
      litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("CommandBuilder::AddChildCommand") {
    SUBCASE("Sets a child command as reference") {
      const auto [file, data] = CreateTOMLMock("test", "");

      litr::Config::CommandBuilder builderRoot{file, data, "test"};
      litr::Config::CommandBuilder builderChild{file, data, "test"};

      builderRoot.AddChildCommand(builderChild.GetResult());

      CHECK_EQ(litr::Error::Handler::GetErrors().size(), 0);
      CHECK_EQ(builderRoot.GetResult()->ChildCommands.size(), 1);
      litr::Error::Handler::Flush();
    }
  }
}
