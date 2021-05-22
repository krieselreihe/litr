#include "Core/Config/CommandBuilder.hpp"

#include <doctest/doctest.h>

#include "Core/Error/Handler.hpp"
#include "Helpers/TOML.hpp"

TEST_SUITE("Config::CommandBuilder") {
  TEST_CASE("Initiates a Command on construction") {
    const auto [file, data] = CreateTOMLMock("test", "");

    Litr::Config::CommandBuilder builder{file, data, "test"};
    Litr::Ref<Litr::Config::Command> builderResult{builder.GetResult()};
    Litr::Config::Command compare{"test"};

    CHECK(Litr::Error::Handler::GetErrors().size() == 0);
    CHECK(sizeof(*builderResult) == sizeof(compare));
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("CommandBuilder::AddScriptLine") {
    SUBCASE("Can add multiple lines of script to the command") {
      const auto [file, data] = CreateTOMLMock("test", "");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddScriptLine("first line");
      builder.AddScriptLine("second line");

      const auto result{builder.GetResult()};

      CHECK(Litr::Error::Handler::GetErrors().size() == 0);
      CHECK(result->Script[0] == "first line");
      CHECK(result->Script[1] == "second line");
      Litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("CommandBuilder::AddScript") {
    SUBCASE("Can override the whole script at once") {
      const auto [file, data] = CreateTOMLMock("test", "");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      const std::vector script{"first line", "second line"};
      builder.AddScript(script);

      const auto result{builder.GetResult()};

      CHECK(Litr::Error::Handler::GetErrors().size() == 0);
      CHECK(result->Script[0] == "first line");
      CHECK(result->Script[1] == "second line");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits and error if script data is not of type string") {
      const auto [file, data] = CreateTOMLMock("test", "scripts = [1]");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddScript(toml::find(data, "scripts"));

      CHECK(Litr::Error::Handler::GetErrors().size() == 1);
      CHECK(Litr::Error::Handler::GetErrors()[0].Message == "A command script can be either a string or array of strings.");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits only one error if script data is not of type string") {
      const auto [file, data] = CreateTOMLMock("test", "scripts = [1, 2, 3]");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddScript(toml::find(data, "scripts"));

      CHECK(Litr::Error::Handler::GetErrors().size() == 1);
      CHECK(Litr::Error::Handler::GetErrors()[0].Message == "A command script can be either a string or array of strings.");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Writes scripts from TOML data successfully") {
      const auto [file, data] = CreateTOMLMock("test", R"(scripts = ["first line", "second line"])");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddScript(toml::find(data, "scripts"));

      const auto result{builder.GetResult()};

      CHECK(Litr::Error::Handler::GetErrors().size() == 0);
      CHECK(result->Script[0] == "first line");
      CHECK(result->Script[1] == "second line");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Retains locations information") {
      const auto [file, data] = CreateTOMLMock("test", R"(scripts = ["first line", "second line"])");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddScript(toml::find(data, "scripts"));

      const auto result{builder.GetResult()};

      CHECK(Litr::Error::Handler::GetErrors().size() == 0);
      CHECK(result->Locations[0].Line == 1);
      CHECK(result->Locations[0].Column == 12);
      CHECK(result->Locations[0].LineStr == R"(scripts = ["first line", "second line"])");
      CHECK(result->Locations[1].Line == 1);
      CHECK(result->Locations[1].Column == 26);
      CHECK(result->Locations[1].LineStr == R"(scripts = ["first line", "second line"])");
      Litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("CommandBuilder::AddDescription") {
    SUBCASE("Does nothing if description is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK(Litr::Error::Handler::GetErrors().size() == 0);
      CHECK(builder.GetResult()->Description.empty());
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits and error if description is not a string") {
      const auto [file, data] = CreateTOMLMock("test", R"(description = 42)");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK(Litr::Error::Handler::GetErrors().size() == 1);
      CHECK(Litr::Error::Handler::GetErrors()[0].Message == R"(The "description" can only be a string.)");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Extracts the description from toml data") {
      const auto [file, data] = CreateTOMLMock("test", R"(description = "Text")");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDescription();

      CHECK(Litr::Error::Handler::GetErrors().size() == 0);
      CHECK(builder.GetResult()->Description == "Text");
      Litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("CommandBuilder::AddExample") {
    SUBCASE("Does nothing if example is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddExample();

      CHECK(Litr::Error::Handler::GetErrors().size() == 0);
      CHECK(builder.GetResult()->Example.empty());
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits and error if example is not a string") {
      const auto [file, data] = CreateTOMLMock("test", R"(example = 42)");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddExample();

      CHECK(Litr::Error::Handler::GetErrors().size() == 1);
      CHECK(Litr::Error::Handler::GetErrors()[0].Message == R"(The "example" can only be a string.)");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Extracts the example from toml data") {
      const auto [file, data] = CreateTOMLMock("test", R"(example = "Text")");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddExample();

      CHECK(Litr::Error::Handler::GetErrors().size() == 0);
      CHECK(builder.GetResult()->Example == "Text");
      Litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("CommandBuilder::AddDirectory") {
    SUBCASE("Does nothing if dir is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDirectory();

      CHECK(Litr::Error::Handler::GetErrors().size() == 0);
      CHECK(builder.GetResult()->Directory.empty());
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if dir is not a string or array of strings") {
      const auto [file, data] = CreateTOMLMock("test", R"(dir = 42)");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDirectory();

      CHECK(Litr::Error::Handler::GetErrors().size() == 1);
      CHECK(Litr::Error::Handler::GetErrors()[0].Message == R"(A "dir" can either be a string or array of strings.)");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if dir array does not only contain strings") {
      const auto [file, data] = CreateTOMLMock("test", R"(dir = [1])");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDirectory();

      CHECK(Litr::Error::Handler::GetErrors().size() == 1);
      CHECK(Litr::Error::Handler::GetErrors()[0].Message == R"(A "dir" can either be a string or array of strings.)");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits more than one error if dir array does not only contain multiple strings") {
      const auto [file, data] = CreateTOMLMock("test", R"(dir = [1, 2])");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDirectory();

      CHECK(Litr::Error::Handler::GetErrors().size() == 2);
      CHECK(Litr::Error::Handler::GetErrors()[0].Message == R"(A "dir" can either be a string or array of strings.)");
      CHECK(Litr::Error::Handler::GetErrors()[1].Message == R"(A "dir" can either be a string or array of strings.)");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Creates a directory folder from a string") {
      const auto [file, data] = CreateTOMLMock("test", R"(dir = ["folder1"])");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDirectory();

      CHECK(Litr::Error::Handler::GetErrors().size() == 0);
      CHECK(builder.GetResult()->Directory[0] == "folder1");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Creates a directory folder from an array of strings") {
      const auto [file, data] = CreateTOMLMock("test", R"(dir = ["folder1", "folder2"])");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddDirectory();

      CHECK(Litr::Error::Handler::GetErrors().size() == 0);
      CHECK(builder.GetResult()->Directory[0] == "folder1");
      CHECK(builder.GetResult()->Directory[1] == "folder2");
      Litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("CommandBuilder::AddOutput") {
    SUBCASE("Does nothing if output is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddOutput();

      CHECK(Litr::Error::Handler::GetErrors().size() == 0);
      CHECK(builder.GetResult()->Output == Litr::Config::Command::Output::UNCHANGED);
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Emits an error if output type is not known") {
      const auto [file, data] = CreateTOMLMock("test", R"(output = "unknown")");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddOutput();

      CHECK(Litr::Error::Handler::GetErrors().size() == 1);
      CHECK(Litr::Error::Handler::GetErrors()[0].Message == R"(The "output" can either be "unchanged" or "silent".)");
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Sets the output to silent if the option is provided") {
      const auto [file, data] = CreateTOMLMock("test", R"(output = "silent")");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddOutput();

      CHECK(Litr::Error::Handler::GetErrors().size() == 0);
      CHECK(builder.GetResult()->Output == Litr::Config::Command::Output::SILENT);
      Litr::Error::Handler::Flush();
    }

    SUBCASE("Sets the output to unchanged if the option is provided") {
      const auto [file, data] = CreateTOMLMock("test", R"(output = "unchanged")");

      Litr::Config::CommandBuilder builder{file, data, "test"};
      builder.AddOutput();

      CHECK(Litr::Error::Handler::GetErrors().size() == 0);
      CHECK(builder.GetResult()->Output == Litr::Config::Command::Output::UNCHANGED);
      Litr::Error::Handler::Flush();
    }
  }

  TEST_CASE("CommandBuilder::AddChildCommand") {
    SUBCASE("Sets a child command as reference") {
      const auto [file, data] = CreateTOMLMock("test", "");

      Litr::Config::CommandBuilder builderRoot{file, data, "test"};
      Litr::Config::CommandBuilder builderChild{file, data, "test"};

      builderRoot.AddChildCommand(builderChild.GetResult());

      CHECK(Litr::Error::Handler::GetErrors().size() == 0);
      CHECK(builderRoot.GetResult()->ChildCommands.size() == 1);
      Litr::Error::Handler::Flush();
    }
  }
}
