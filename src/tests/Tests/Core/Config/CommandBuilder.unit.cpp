/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/Config/CommandBuilder.hpp"

#include <doctest/doctest.h>

#include "Core/Error/Handler.hpp"
#include "Helpers/TOML.hpp"

TEST_SUITE("Config::CommandBuilder") {
  TEST_CASE("Initiates a Command on construction") {
    const auto [context, data] = create_toml_mock("test", "");

    const Litr::Config::CommandBuilder builder{context, data, "test"};
    const Litr::Config::Command builder_result{*builder.result()};
    const Litr::Config::Command compare{"test"};

    CHECK_EQ(Litr::Error::Handler::errors().size(), 0);
    CHECK_EQ(sizeof(builder_result), sizeof(compare));
    Litr::Error::Handler::flush();
  }

  TEST_CASE("CommandBuilder::add_script_line") {
    SUBCASE("Can add multiple lines of script to the command") {
      const auto [context, data] = create_toml_mock("test", "");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_script_line("first line");
      builder.add_script_line("second line");

      const auto result{builder.result()};

      CHECK_EQ(Litr::Error::Handler::errors().size(), 0);
      CHECK_EQ(result->script[0], "first line");
      CHECK_EQ(result->script[1], "second line");
      Litr::Error::Handler::flush();
    }
  }

  TEST_CASE("CommandBuilder::add_script") {
    SUBCASE("Can override the whole script at once") {
      const auto [context, data] = create_toml_mock("test", "");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      const std::vector script{"first line", "second line"};
      builder.add_script(script);

      const auto result{builder.result()};

      CHECK_EQ(Litr::Error::Handler::errors().size(), 0);
      CHECK_EQ(result->script[0], "first line");
      CHECK_EQ(result->script[1], "second line");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits and error if script data is not of type string") {
      const auto [context, data] = create_toml_mock("test", "scripts = [1]");

      const Litr::Config::TomlFileAdapter file{};
      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_script(file.find(data, "scripts"));

      CHECK_EQ(Litr::Error::Handler::errors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::errors()[0].message,
          "A command script can be either a string or array of strings.");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits only one error if script data is not of type string") {
      const auto [context, data] = create_toml_mock("test", "scripts = [1, 2, 3]");

      const Litr::Config::TomlFileAdapter file{};
      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_script(file.find(data, "scripts"));

      CHECK_EQ(Litr::Error::Handler::errors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::errors()[0].message,
          "A command script can be either a string or array of strings.");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Writes scripts from TOML data successfully") {
      const auto [context, data] =
          create_toml_mock("test", R"(scripts = ["first line", "second line"])");

      const Litr::Config::TomlFileAdapter file{};
      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_script(file.find(data, "scripts"));

      const auto result{builder.result()};

      CHECK_EQ(Litr::Error::Handler::errors().size(), 0);
      CHECK_EQ(result->script[0], "first line");
      CHECK_EQ(result->script[1], "second line");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Retains locations information") {
      const auto [context, data] =
          create_toml_mock("test", R"(scripts = ["first line", "second line"])");

      const Litr::Config::TomlFileAdapter file{};
      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_script(file.find(data, "scripts"));

      const auto result{builder.result()};

      CHECK_EQ(Litr::Error::Handler::errors().size(), 0);
      CHECK_EQ(result->locations[0].line, 1);
      CHECK_EQ(result->locations[0].column, 12);
      CHECK_EQ(result->locations[0].line_str, R"(scripts = ["first line", "second line"])");
      CHECK_EQ(result->locations[1].line, 1);
      CHECK_EQ(result->locations[1].column, 26);
      CHECK_EQ(result->locations[1].line_str, R"(scripts = ["first line", "second line"])");
      Litr::Error::Handler::flush();
    }
  }

  TEST_CASE("CommandBuilder::add_description") {
    SUBCASE("Does nothing if description is not set") {
      const auto [context, data] = create_toml_mock("test", R"(key = "value")");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_description();

      CHECK_EQ(Litr::Error::Handler::errors().size(), 0);
      CHECK(builder.result()->description.empty());
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits and error if description is not a string") {
      const auto [context, data] = create_toml_mock("test", R"(description = 42)");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_description();

      CHECK_EQ(Litr::Error::Handler::errors().size(), 1);
      CHECK_EQ(
          Litr::Error::Handler::errors()[0].message, R"(The "description" can only be a string.)");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Extracts the description from toml data") {
      const auto [context, data] = create_toml_mock("test", R"(description = "Text")");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_description();

      CHECK_EQ(Litr::Error::Handler::errors().size(), 0);
      CHECK_EQ(builder.result()->description, "Text");
      Litr::Error::Handler::flush();
    }
  }

  TEST_CASE("CommandBuilder::add_example") {
    SUBCASE("Does nothing if example is not set") {
      const auto [context, data] = create_toml_mock("test", R"(key = "value")");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_example();

      CHECK_EQ(Litr::Error::Handler::errors().size(), 0);
      CHECK(builder.result()->example.empty());
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits and error if example is not a string") {
      const auto [context, data] = create_toml_mock("test", R"(example = 42)");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_example();

      CHECK_EQ(Litr::Error::Handler::errors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::errors()[0].message, R"(The "example" can only be a string.)");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Extracts the example from toml data") {
      const auto [context, data] = create_toml_mock("test", R"(example = "Text")");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_example();

      CHECK_EQ(Litr::Error::Handler::errors().size(), 0);
      CHECK_EQ(builder.result()->example, "Text");
      Litr::Error::Handler::flush();
    }
  }

  TEST_CASE("CommandBuilder::add_directory") {
    SUBCASE("Does nothing if dir is not set") {
      const auto [context, data] = create_toml_mock("test", R"(key = "value")");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_directory(Litr::Path(""));

      CHECK_EQ(Litr::Error::Handler::errors().size(), 0);
      CHECK(builder.result()->directory.empty());
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits an error if dir is not a string or array of strings") {
      const auto [context, data] = create_toml_mock("test", R"(dir = 42)");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_directory(Litr::Path(""));

      CHECK_EQ(Litr::Error::Handler::errors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::errors()[0].message,
          R"(A "dir" can either be a string or array of strings.)");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits an error if dir array does not only contain strings") {
      const auto [context, data] = create_toml_mock("test", R"(dir = [1])");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_directory(Litr::Path(""));

      CHECK_EQ(Litr::Error::Handler::errors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::errors()[0].message,
          R"(A "dir" can either be a string or array of strings.)");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits more than one error if dir array does not only contain multiple strings") {
      const auto [context, data] = create_toml_mock("test", R"(dir = [1, 2])");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_directory(Litr::Path(""));

      CHECK_EQ(Litr::Error::Handler::errors().size(), 2);
      CHECK_EQ(Litr::Error::Handler::errors()[0].message,
          R"(A "dir" can either be a string or array of strings.)");
      CHECK_EQ(Litr::Error::Handler::errors()[1].message,
          R"(A "dir" can either be a string or array of strings.)");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Creates a directory folder from a string") {
      const auto [context, data] = create_toml_mock("test", R"(dir = ["folder1"])");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_directory(Litr::Path(""));

      CHECK_EQ(Litr::Error::Handler::errors().size(), 0);
      CHECK_EQ(builder.result()->directory[0], "folder1");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Creates a directory folder from an array of strings") {
      const auto [context, data] = create_toml_mock("test", R"(dir = ["folder1", "folder2"])");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_directory(Litr::Path(""));

      CHECK_EQ(Litr::Error::Handler::errors().size(), 0);
      CHECK_EQ(builder.result()->directory[0], "folder1");
      CHECK_EQ(builder.result()->directory[1], "folder2");
      Litr::Error::Handler::flush();
    }
  }

  TEST_CASE("CommandBuilder::add_output") {
    SUBCASE("Does nothing if output is not set") {
      const auto [context, data] = create_toml_mock("test", R"(key = "value")");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_output();

      CHECK_EQ(Litr::Error::Handler::errors().size(), 0);
      CHECK_EQ(builder.result()->output, Litr::Config::Command::Output::UNCHANGED);
      Litr::Error::Handler::flush();
    }

    SUBCASE("Emits an error if output type is not known") {
      const auto [context, data] = create_toml_mock("test", R"(output = "unknown")");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_output();

      CHECK_EQ(Litr::Error::Handler::errors().size(), 1);
      CHECK_EQ(Litr::Error::Handler::errors()[0].message,
          R"(The "output" can either be "unchanged" or "silent".)");
      Litr::Error::Handler::flush();
    }

    SUBCASE("Sets the output to silent if the option is provided") {
      const auto [context, data] = create_toml_mock("test", R"(output = "silent")");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_output();

      CHECK_EQ(Litr::Error::Handler::errors().size(), 0);
      CHECK_EQ(builder.result()->output, Litr::Config::Command::Output::SILENT);
      Litr::Error::Handler::flush();
    }

    SUBCASE("Sets the output to unchanged if the option is provided") {
      const auto [context, data] = create_toml_mock("test", R"(output = "unchanged")");

      Litr::Config::CommandBuilder builder{context, data, "test"};
      builder.add_output();

      CHECK_EQ(Litr::Error::Handler::errors().size(), 0);
      CHECK_EQ(builder.result()->output, Litr::Config::Command::Output::UNCHANGED);
      Litr::Error::Handler::flush();
    }
  }

  TEST_CASE("CommandBuilder::add_child_command") {
    SUBCASE("Sets a child command as reference") {
      const auto [context, data] = create_toml_mock("test", "");

      Litr::Config::CommandBuilder builder_root{context, data, "test"};
      const Litr::Config::CommandBuilder builder_child{context, data, "test"};

      builder_root.add_child_command(builder_child.result());

      CHECK_EQ(Litr::Error::Handler::errors().size(), 0);
      CHECK_EQ(builder_root.result()->child_commands.size(), 1);
      Litr::Error::Handler::flush();
    }
  }
}
