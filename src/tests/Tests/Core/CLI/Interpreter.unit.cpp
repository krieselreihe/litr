/*
 * Copyright (c) 2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/CLI/Interpreter.hpp"

#include <doctest/doctest.h>

#include <memory>

#include "Core/CLI/Parser.hpp"

TEST_SUITE("CLI::Interpreter") {
  TEST_CASE("command_path_to_human_readable()") {
    SUBCASE("Does convert a command path to a human readable form") {
      const std::string result{
          Litr::CLI::Interpreter::command_path_to_human_readable("some.command.path")};
      CHECK_EQ(result, "some command path");
    }
  }

  TEST_CASE("command_name_occurrence()") {
    SUBCASE("Returns the command name until a specified parameter name was found") {
      const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
      const std::string source{"run a command --stop"};
      const Litr::CLI::Parser parser{instruction, source};
      const auto config{std::make_shared<Litr::Config::Loader>(
          Litr::Path("../../Fixtures/Config/full-example.toml"))};
      const Litr::CLI::Interpreter interpreter{instruction, config};

      const std::string command_name{interpreter.command_name_occurrence({"stop"})};
      CHECK_EQ(command_name, "run.a.command");
    }

    SUBCASE("Returns an empty string if parameter name was not found") {
      const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
      const std::string source{"run a command --stop maybe"};
      const Litr::CLI::Parser parser{instruction, source};
      const auto config{std::make_shared<Litr::Config::Loader>(
          Litr::Path("../../Fixtures/Config/full-example.toml"))};
      const Litr::CLI::Interpreter interpreter{instruction, config};

      const std::string command_name{interpreter.command_name_occurrence({"nothing"})};
      CHECK_EQ(command_name, "");
    }
  }
}
