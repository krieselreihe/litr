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

    litr::config::CommandBuilder builder{file, data, "test"};
    std::shared_ptr<litr::config::Command> builderResult{builder.get_result()};
    litr::config::Command compare{"test"};

    CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
    CHECK_EQ(sizeof(*builderResult), sizeof(compare));
    litr::error::Handler::flush();
  }

  TEST_CASE("CommandBuilder::add_script_line") {
    SUBCASE("Can add multiple lines of script to the command") {
      const auto [file, data] = CreateTOMLMock("test", "");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_script_line("first line");
      builder.add_script_line("second line");

      const auto result{builder.get_result()};

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(result->script[0], "first line");
      CHECK_EQ(result->script[1], "second line");
      litr::error::Handler::flush();
    }
  }

  TEST_CASE("CommandBuilder::add_script") {
    SUBCASE("Can override the whole script at once") {
      const auto [file, data] = CreateTOMLMock("test", "");

      litr::config::CommandBuilder builder{file, data, "test"};
      const std::vector script{"first line", "second line"};
      builder.add_script(script);

      const auto result{builder.get_result()};

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(result->script[0], "first line");
      CHECK_EQ(result->script[1], "second line");
      litr::error::Handler::flush();
    }

    SUBCASE("Emits and error if script data is not of type string") {
      const auto [file, data] = CreateTOMLMock("test", "scripts = [1]");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_script(toml::find(data, "scripts"));

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message, "A command script can be either a string or array of strings.");
      litr::error::Handler::flush();
    }

    SUBCASE("Emits only one error if script data is not of type string") {
      const auto [file, data] = CreateTOMLMock("test", "scripts = [1, 2, 3]");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_script(toml::find(data, "scripts"));

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message, "A command script can be either a string or array of strings.");
      litr::error::Handler::flush();
    }

    SUBCASE("Writes scripts from TOML data successfully") {
      const auto [file, data] = CreateTOMLMock("test", R"(scripts = ["first line", "second line"])");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_script(toml::find(data, "scripts"));

      const auto result{builder.get_result()};

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(result->script[0], "first line");
      CHECK_EQ(result->script[1], "second line");
      litr::error::Handler::flush();
    }

    SUBCASE("Retains locations information") {
      const auto [file, data] = CreateTOMLMock("test", R"(scripts = ["first line", "second line"])");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_script(toml::find(data, "scripts"));

      const auto result{builder.get_result()};

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(result->Locations[0].line, 1);
      CHECK_EQ(result->Locations[0].column, 12);
      CHECK_EQ(result->Locations[0].line_str, R"(scripts = ["first line", "second line"])");
      CHECK_EQ(result->Locations[1].line, 1);
      CHECK_EQ(result->Locations[1].column, 26);
      CHECK_EQ(result->Locations[1].line_str, R"(scripts = ["first line", "second line"])");
      litr::error::Handler::flush();
    }
  }

  TEST_CASE("CommandBuilder::add_description") {
    SUBCASE("Does nothing if description is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_description();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK(builder.get_result()->description.empty());
      litr::error::Handler::flush();
    }

    SUBCASE("Emits and error if description is not a string") {
      const auto [file, data] = CreateTOMLMock("test", R"(description = 42)");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_description();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message, R"(The "description" can only be a string.)");
      litr::error::Handler::flush();
    }

    SUBCASE("Extracts the description from toml data") {
      const auto [file, data] = CreateTOMLMock("test", R"(description = "Text")");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_description();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->description, "Text");
      litr::error::Handler::flush();
    }
  }

  TEST_CASE("CommandBuilder::add_example") {
    SUBCASE("Does nothing if example is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_example();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK(builder.get_result()->example.empty());
      litr::error::Handler::flush();
    }

    SUBCASE("Emits and error if example is not a string") {
      const auto [file, data] = CreateTOMLMock("test", R"(example = 42)");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_example();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message, R"(The "example" can only be a string.)");
      litr::error::Handler::flush();
    }

    SUBCASE("Extracts the example from toml data") {
      const auto [file, data] = CreateTOMLMock("test", R"(example = "Text")");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_example();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->example, "Text");
      litr::error::Handler::flush();
    }
  }

  TEST_CASE("CommandBuilder::add_directory") {
    SUBCASE("Does nothing if dir is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_directory(litr::Path(""));

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK(builder.get_result()->directory.empty());
      litr::error::Handler::flush();
    }

    SUBCASE("Emits an error if dir is not a string or array of strings") {
      const auto [file, data] = CreateTOMLMock("test", R"(dir = 42)");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_directory(litr::Path(""));

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message, R"(A "dir" can either be a string or array of strings.)");
      litr::error::Handler::flush();
    }

    SUBCASE("Emits an error if dir array does not only contain strings") {
      const auto [file, data] = CreateTOMLMock("test", R"(dir = [1])");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_directory(litr::Path(""));

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message, R"(A "dir" can either be a string or array of strings.)");
      litr::error::Handler::flush();
    }

    SUBCASE("Emits more than one error if dir array does not only contain multiple strings") {
      const auto [file, data] = CreateTOMLMock("test", R"(dir = [1, 2])");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_directory(litr::Path(""));

      CHECK_EQ(litr::error::Handler::get_errors().size(), 2);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message, R"(A "dir" can either be a string or array of strings.)");
      CHECK_EQ(litr::error::Handler::get_errors()[1].message, R"(A "dir" can either be a string or array of strings.)");
      litr::error::Handler::flush();
    }

    SUBCASE("Creates a directory folder from a string") {
      const auto [file, data] = CreateTOMLMock("test", R"(dir = ["folder1"])");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_directory(litr::Path(""));

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->directory[0], "folder1");
      litr::error::Handler::flush();
    }

    SUBCASE("Creates a directory folder from an array of strings") {
      const auto [file, data] = CreateTOMLMock("test", R"(dir = ["folder1", "folder2"])");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_directory(litr::Path(""));

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->directory[0], "folder1");
      CHECK_EQ(builder.get_result()->directory[1], "folder2");
      litr::error::Handler::flush();
    }
  }

  TEST_CASE("CommandBuilder::add_output") {
    SUBCASE("Does nothing if output is not set") {
      const auto [file, data] = CreateTOMLMock("test", R"(key = "value")");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_output();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->output, litr::config::Command::Output::UNCHANGED);
      litr::error::Handler::flush();
    }

    SUBCASE("Emits an error if output type is not known") {
      const auto [file, data] = CreateTOMLMock("test", R"(output = "unknown")");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_output();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 1);
      CHECK_EQ(litr::error::Handler::get_errors()[0].message, R"(The "output" can either be "unchanged" or "silent".)");
      litr::error::Handler::flush();
    }

    SUBCASE("Sets the output to silent if the option is provided") {
      const auto [file, data] = CreateTOMLMock("test", R"(output = "silent")");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_output();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->output, litr::config::Command::Output::SILENT);
      litr::error::Handler::flush();
    }

    SUBCASE("Sets the output to unchanged if the option is provided") {
      const auto [file, data] = CreateTOMLMock("test", R"(output = "unchanged")");

      litr::config::CommandBuilder builder{file, data, "test"};
      builder.add_output();

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(builder.get_result()->output, litr::config::Command::Output::UNCHANGED);
      litr::error::Handler::flush();
    }
  }

  TEST_CASE("CommandBuilder::add_child_command") {
    SUBCASE("Sets a child command as reference") {
      const auto [file, data] = CreateTOMLMock("test", "");

      litr::config::CommandBuilder builderRoot{file, data, "test"};
      litr::config::CommandBuilder builderChild{file, data, "test"};

      builderRoot.add_child_command(builderChild.get_result());

      CHECK_EQ(litr::error::Handler::get_errors().size(), 0);
      CHECK_EQ(builderRoot.get_result()->child_commands.size(), 1);
      litr::error::Handler::flush();
    }
  }
}
