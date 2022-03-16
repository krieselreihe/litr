/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/Config/Loader.hpp"

#include <doctest/doctest.h>

#include <memory>

#include "Core/Config/Query.hpp"
#include "Core/Error/Handler.hpp"
#include "Core/FileSystem.hpp"

TEST_SUITE("Config::Loader") {
  TEST_CASE("Loads a config file without issues") {
    const litr::Path path{"../../Fixtures/Config/empty.toml"};
    const litr::config::Loader config{path};

    CHECK_FALSE(litr::error::Handler::has_errors());
    litr::error::Handler::flush();
  }

  TEST_CASE("Emits a custom syntax error on exception") {
    const litr::Path path{"../../Fixtures/Config/syntax-error.toml"};
    const litr::config::Loader config{path};
    const auto errors{litr::error::Handler::get_errors()};

    CHECK(litr::error::Handler::has_errors());
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "There is a syntax error inside the configuration file.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Emits an error on malformed command") {
    const litr::Path path{"../../Fixtures/Config/malformed-command.toml"};
    const litr::config::Loader config{path};
    const auto errors{litr::error::Handler::get_errors()};

    CHECK(litr::error::Handler::has_errors());
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "A command can be a string or table.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Emits an error if command script is not a string") {
    const litr::Path path{"../../Fixtures/Config/malformed-command-script.toml"};
    const litr::config::Loader config{path};
    const auto errors{litr::error::Handler::get_errors()};

    CHECK(litr::error::Handler::has_errors());
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "A command script can be either a string or array of strings.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Emits an error if command property unknown") {
    const litr::Path path{"../../Fixtures/Config/unknown-command-property.toml"};
    const litr::config::Loader config{path};
    const auto errors{litr::error::Handler::get_errors()};

    CHECK(litr::error::Handler::has_errors());
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message,
        R"(The command property "unknown" does not exist. Please refer to the docs.)");
    litr::error::Handler::flush();
  }

  TEST_CASE("Copies errors from CommandBuilder to Loader on malformed script array") {
    const litr::Path path{"../../Fixtures/Config/command-script-array-malformed.toml"};
    const litr::config::Loader config{path};
    const auto errors{litr::error::Handler::get_errors()};

    CHECK(litr::error::Handler::has_errors());
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "A command script can be either a string or array of strings.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Copies errors from CommandBuilder to Loader on detailed malformed script array") {
    const litr::Path path{"../../Fixtures/Config/command-detailed-script-array-malformed.toml"};
    const litr::config::Loader config{path};
    const auto errors{litr::error::Handler::get_errors()};

    CHECK(litr::error::Handler::has_errors());
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "A command script can be either a string or array of strings.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Copies errors from CommandBuilder to Loader on multiple malformed fields") {
    const litr::Path path{"../../Fixtures/Config/command-description-and-output-malformed.toml"};
    const litr::config::Loader config{path};
    const auto errors{litr::error::Handler::get_errors()};

    CHECK(litr::error::Handler::has_errors());
    CHECK_EQ(errors.size(), 2);
    CHECK_EQ(errors[0].message, R"(The "description" can only be a string.)");
    CHECK_EQ(errors[1].message, R"(The "output" can either be "unchanged" or "silent".)");
    litr::error::Handler::flush();
  }

  TEST_CASE("Does nothing if no commands or parameters defined") {
    const litr::Path path{"../../Fixtures/Config/empty-commands-params.toml"};
    const litr::config::Loader config{path};

    CHECK_FALSE(litr::error::Handler::has_errors());
    CHECK_EQ(config.get_commands().size(), 0);
    CHECK_EQ(config.get_parameters().size(), 0);
    litr::error::Handler::flush();
  }

  TEST_CASE("Loads commands") {
    const litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const auto config{std::make_shared<litr::config::Loader>(path)};

    SUBCASE("Successfully without errors") {
      CHECK_FALSE(litr::error::Handler::has_errors());
      CHECK_EQ(config->get_commands().size(), 3);
    }

    SUBCASE("Resolves all fields on a command") {
      const litr::config::Query query{config};
      const auto command{query.get_command("run")};

      CHECK_EQ(command->name, "run");
      CHECK_EQ(command->script.size(), 1);
      CHECK_EQ(command->script[0], "Script");
      CHECK_EQ(command->description, "Description");
      CHECK_EQ(command->example, "Example");
      CHECK_EQ(command->output, litr::config::Command::Output::SILENT);
      CHECK_EQ(command->directory.size(), 1);
      CHECK_EQ(command->directory[0], "../../Fixtures/Config/Directory");
      CHECK_EQ(command->child_commands.size(), 4);
    }

    SUBCASE("Resolves all fields on a command without sub commands") {
      const litr::config::Query query{config};
      const auto command{query.get_command("update")};

      CHECK_EQ(command->name, "update");
      CHECK_EQ(command->script.size(), 1);
      CHECK_EQ(command->script[0], "git pull && git submodule update --init");
      CHECK(command->description.empty());
      CHECK(command->example.empty());
      CHECK_EQ(command->output, litr::config::Command::Output::UNCHANGED);
      CHECK(command->directory.empty());
      CHECK(command->child_commands.empty());
    }

    SUBCASE("Resolves all fields on a sub command") {
      const litr::config::Query query{config};
      const auto command1{query.get_command("run.first")};
      const auto command2{query.get_command("run.second")};
      const auto command3{query.get_command("run.third")};
      const auto command4{query.get_command("run.fourth")};

      CHECK_NE(command1, nullptr);
      CHECK_NE(command2, nullptr);
      CHECK_NE(command3, nullptr);
      CHECK_NE(command4, nullptr);

      CHECK_EQ(command1->name, "first");
      CHECK_EQ(command2->name, "second");
      CHECK_EQ(command3->name, "third");
      CHECK_EQ(command4->name, "fourth");

      CHECK_EQ(command1->script.size(), 1);
      CHECK_EQ(command1->script[0], "run first");
      CHECK_EQ(command2->script.size(), 1);
      CHECK_EQ(command2->script[0], "run second");
      CHECK_EQ(command3->script.size(), 2);
      CHECK_EQ(command3->script[0], "Script1");
      CHECK_EQ(command3->script[1], "Script2");
      CHECK_EQ(command4->script.size(), 1);
      CHECK_EQ(command4->script[0], "run fourth");

      CHECK_EQ(command3->description, "Description");
      CHECK_EQ(command3->directory.size(), 2);
      CHECK_EQ(command3->directory[0], "../../Fixtures/Config/Directory1");
      CHECK_EQ(command3->directory[1], "../../Fixtures/Config/Directory2");
    }

    SUBCASE("Resolves a very deep nested script") {
      const litr::config::Query query{config};
      const auto command{query.get_command("run.fourth.l1.l2.l3")};

      CHECK_NE(command, nullptr);
      CHECK_EQ(command->name, "l3");
      CHECK_EQ(command->script.size(), 1);
      CHECK_EQ(command->script[0], "Deep Script");
    }
  }

  TEST_CASE("Emits an error if parameter name is reserved for Litr") {
    const litr::Path path{"../../Fixtures/Config/reserved-parameter.toml"};
    const litr::config::Loader config{path};
    const auto errors{litr::error::Handler::get_errors()};

    CHECK(litr::error::Handler::has_errors());
    CHECK_EQ(errors.size(), 2);
    CHECK_EQ(errors[0].message, R"(The parameter name "h" is reserved by Litr.)");
    CHECK_EQ(errors[1].message, R"(The parameter name "help" is reserved by Litr.)");
    litr::error::Handler::flush();
  }

  TEST_CASE("Emits an error if parameter definition is malformed") {
    const litr::Path path{"../../Fixtures/Config/malformed-parameter.toml"};
    const litr::config::Loader config{path};
    const auto errors{litr::error::Handler::get_errors()};

    CHECK(litr::error::Handler::has_errors());
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "A parameter needs to be a string or table.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Copies errors from ParameterBuilder to Loader on multiple malformed params") {
    const litr::Path path{"../../Fixtures/Config/params-description-and-type-malformed.toml"};
    const litr::config::Loader config{path};
    const auto errors{litr::error::Handler::get_errors()};

    CHECK(litr::error::Handler::has_errors());
    CHECK_EQ(errors.size(), 2);
    CHECK_EQ(errors[0].message, R"(The "description" can only be a string.)");
    CHECK_EQ(
        errors[1].message, R"(A "type" can only be "string" or an array of options as strings.)");
    litr::error::Handler::flush();
  }

  TEST_CASE("Loads Parameters") {
    const litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const auto config{std::make_shared<litr::config::Loader>(path)};

    SUBCASE("Successfully without errors") {
      CHECK_FALSE(litr::error::Handler::has_errors());
      CHECK_EQ(config->get_parameters().size(), 2);
    }

    SUBCASE("Resolves all fields on a parameter") {
      const litr::config::Query query{config};
      const auto param{query.get_parameter("target")};

      CHECK_EQ(param->name, "target");
      CHECK_EQ(param->description, "Description");
      CHECK_EQ(param->shortcut, "t");
      CHECK_EQ(param->default_value, "debug");
      CHECK_EQ(param->type, litr::config::Parameter::Type::ARRAY);
      CHECK_EQ(param->type_arguments.size(), 2);
      CHECK_EQ(param->type_arguments[0], "debug");
      CHECK_EQ(param->type_arguments[1], "release");
    }
  }
}
