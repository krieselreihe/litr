#include <doctest/doctest.h>

#include "Helpers/TOML.hpp"
#include "Core/Errors/ErrorHandler.hpp"
#include "Core/Config/CommandBuilder.hpp"

TEST_SUITE("CommandBuilder") {
  TEST_CASE("Initiates a Command on construction") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto [file, data] = CreateTOMLMock("test", "");

    Litr::CommandBuilder builder{errorHandler, file, data, "test"};
    Litr::Ref<Litr::Command> builderResult{builder.GetResult()};
    Litr::Command compare{"test"};

    CHECK(errorHandler->GetErrors().size() == 0);
    CHECK(sizeof(*builderResult) == sizeof(compare));
  }

  TEST_CASE("CommandBuilder::AddScriptLine") {
    SUBCASE("Can add multiple lines of script to the command") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", "");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddScriptLine("first line");
      builder.AddScriptLine("second line");

      const auto result{builder.GetResult()};

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(result->Script[0] == "first line");
      CHECK(result->Script[1] == "second line");
    }
  }

  TEST_CASE("CommandBuilder::AddScript") {
    SUBCASE("Can override the whole script at once") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", "");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      const std::vector script{"first line", "second line"};
      builder.AddScript(script);

      const auto result{builder.GetResult()};

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(result->Script[0] == "first line");
      CHECK(result->Script[1] == "second line");
    }

    SUBCASE("Emits and error if script data is not of type string") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", "scripts = [1]");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddScript(toml::find(data, "scripts"));

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == "A command script can be either a string or array of strings.");
    }

    SUBCASE("Emits only one error if script data is not of type string") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", "scripts = [1, 2, 3]");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddScript(toml::find(data, "scripts"));

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == "A command script can be either a string or array of strings.");
    }

    SUBCASE("Writes scripts from TOML data successfully") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(scripts = ["first line", "second line"])");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddScript(toml::find(data, "scripts"));

      const auto result{builder.GetResult()};

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(result->Script[0] == "first line");
      CHECK(result->Script[1] == "second line");
    }
  }

  TEST_CASE("CommandBuilder::AddDescription") {
    SUBCASE("Does nothing if description is not set") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddDescription();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Description.empty());
    }

    SUBCASE("Emits and error if description is not a string") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(description = 42)");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddDescription();

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == R"(The "description" can can only be a string.)");
    }

    SUBCASE("Extracts the description from toml data") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(description = "Text")");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddDescription();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Description == "Text");
    }
  }

  TEST_CASE("CommandBuilder::AddExample") {
    SUBCASE("Does nothing if example is not set") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddExample();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Example.empty());
    }

    SUBCASE("Emits and error if example is not a string") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(example = 42)");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddExample();

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == R"(The "example" can can only be a string.)");
    }

    SUBCASE("Extracts the example from toml data") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(example = "Text")");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddExample();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Example == "Text");
    }
  }

  TEST_CASE("CommandBuilder::AddDirectory") {
    SUBCASE("Does nothing if dir is not set") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddDirectory();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Directory.empty());
    }

    SUBCASE("Emits an error if dir is not a string or array of strings") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(dir = 42)");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddDirectory();

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == R"(A "dir" can either be a string or array of strings.)");
    }

    SUBCASE("Emits an error if dir array does not only contain strings") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(dir = [1])");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddDirectory();

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == R"(A "dir" can either be a string or array of strings.)");
    }

    SUBCASE("Emits more than one error if dir array does not only contain multiple strings") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(dir = [1, 2])");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddDirectory();

      CHECK(errorHandler->GetErrors().size() == 2);
      CHECK(errorHandler->GetErrors()[0].Message == R"(A "dir" can either be a string or array of strings.)");
      CHECK(errorHandler->GetErrors()[1].Message == R"(A "dir" can either be a string or array of strings.)");
    }

    SUBCASE("Creates a directory folder from a string") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(dir = ["folder1"])");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddDirectory();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Directory[0] == "folder1");
    }

    SUBCASE("Creates a directory folder from an array of strings") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(dir = ["folder1", "folder2"])");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddDirectory();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Directory[0] == "folder1");
      CHECK(builder.GetResult()->Directory[1] == "folder2");
    }
  }

  TEST_CASE("CommandBuilder::AddOutput") {
    SUBCASE("Does nothing if output is not set") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddOutput();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Output == Litr::Command::Output::UNCHANGED);
    }

    SUBCASE("Emits an error if output type is not known") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(output = "unknown")");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddOutput();

      CHECK(errorHandler->GetErrors().size() == 1);
      CHECK(errorHandler->GetErrors()[0].Message == R"(The "output" can either be "unchanged" or "silent".)");
    }

    SUBCASE("Sets the output to silent if the option is provided") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(output = "silent")");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddOutput();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Output == Litr::Command::Output::SILENT);
    }

    SUBCASE("Sets the output to unchanged if the option is provided") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", R"(output = "unchanged")");

      Litr::CommandBuilder builder{errorHandler, file, data, "test"};
      builder.AddOutput();

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builder.GetResult()->Output == Litr::Command::Output::UNCHANGED);
    }
  }

  TEST_CASE("CommandBuilder::AddChildCommand") {
    SUBCASE("Sets a child command as reference") {
      const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
      const auto [file, data] = CreateTOMLMock("test", "");

      Litr::CommandBuilder builderRoot{errorHandler, file, data, "test"};
      Litr::CommandBuilder builderChild{errorHandler, file, data, "test"};

      builderRoot.AddChildCommand(builderChild.GetResult());

      CHECK(errorHandler->GetErrors().size() == 0);
      CHECK(builderRoot.GetResult()->ChildCommands.size() == 1);
    }
  }
}
