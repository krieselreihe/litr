/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/Config/Loader.hpp"

#include <doctest/doctest.h>

#include "Core/Config/Query.hpp"
#include "Core/Error/Handler.hpp"
#include "Core/FileSystem.hpp"

TEST_SUITE("Config::Loader") {
  TEST_CASE("Loads a config file without issues") {
    const litr::Path path{"../../Fixtures/Config/empty.toml"};
    const litr::Config::Loader config{path};

    CHECK_FALSE(litr::Error::Handler::HasErrors());
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Emits a custom syntax error on exception") {
    const litr::Path path{"../../Fixtures/Config/syntax-error.toml"};
    const litr::Config::Loader config{path};
    const auto errors{litr::Error::Handler::GetErrors()};

    CHECK(litr::Error::Handler::HasErrors());
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "There is a syntax error inside the configuration file.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Emits an error on malformed command") {
    const litr::Path path{"../../Fixtures/Config/malformed-command.toml"};
    const litr::Config::Loader config{path};
    const auto errors{litr::Error::Handler::GetErrors()};

    CHECK(litr::Error::Handler::HasErrors());
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "A command can be a string or table.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Emits an error if command script is not a string") {
    const litr::Path path{"../../Fixtures/Config/malformed-command-script.toml"};
    const litr::Config::Loader config{path};
    const auto errors{litr::Error::Handler::GetErrors()};

    CHECK(litr::Error::Handler::HasErrors());
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "A command script can be either a string or array of strings.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Emits an error if command property unknown") {
    const litr::Path path{"../../Fixtures/Config/unknown-command-property.toml"};
    const litr::Config::Loader config{path};
    const auto errors{litr::Error::Handler::GetErrors()};

    CHECK(litr::Error::Handler::HasErrors());
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, R"(The command property "unknown" does not exist. Please refer to the docs.)");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Copies errors from CommandBuilder to Loader on malformed script array") {
    const litr::Path path{"../../Fixtures/Config/command-script-array-malformed.toml"};
    const litr::Config::Loader config{path};
    const auto errors{litr::Error::Handler::GetErrors()};

    CHECK(litr::Error::Handler::HasErrors());
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "A command script can be either a string or array of strings.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Copies errors from CommandBuilder to Loader on detailed malformed script array") {
    const litr::Path path{"../../Fixtures/Config/command-detailed-script-array-malformed.toml"};
    const litr::Config::Loader config{path};
    const auto errors{litr::Error::Handler::GetErrors()};

    CHECK(litr::Error::Handler::HasErrors());
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "A command script can be either a string or array of strings.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Copies errors from CommandBuilder to Loader on multiple malformed fields") {
    const litr::Path path{"../../Fixtures/Config/command-description-and-output-malformed.toml"};
    const litr::Config::Loader config{path};
    const auto errors{litr::Error::Handler::GetErrors()};

    CHECK(litr::Error::Handler::HasErrors());
    CHECK_EQ(errors.size(), 2);
    CHECK_EQ(errors[0].Message, R"(The "description" can only be a string.)");
    CHECK_EQ(errors[1].Message, R"(The "output" can either be "unchanged" or "silent".)");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Does nothing if no commands or parameters defined") {
    const litr::Path path{"../../Fixtures/Config/empty-commands-params.toml"};
    const litr::Config::Loader config{path};

    CHECK_FALSE(litr::Error::Handler::HasErrors());
    CHECK_EQ(config.GetCommands().size(), 0);
    CHECK_EQ(config.GetParameters().size(), 0);
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Loads commands") {
    const litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const auto config{litr::CreateRef<litr::Config::Loader>(path)};

    SUBCASE("Successfully without errors") {
      CHECK_FALSE(litr::Error::Handler::HasErrors());
      CHECK_EQ(config->GetCommands().size(), 3);
    }

    SUBCASE("Resolves all fields on a command") {
      const litr::Config::Query query{config};
      const auto command{query.GetCommand("run")};

      CHECK_EQ(command->Name, "run");
      CHECK_EQ(command->Script.size(), 1);
      CHECK_EQ(command->Script[0], "Script");
      CHECK_EQ(command->Description, "Description");
      CHECK_EQ(command->Example, "Example");
      CHECK_EQ(command->Output, litr::Config::Command::Output::SILENT);
      CHECK_EQ(command->Directory.size(), 1);
      CHECK_EQ(command->Directory[0], "../../Fixtures/Config/Directory");
      CHECK_EQ(command->ChildCommands.size(), 4);
    }

    SUBCASE("Resolves all fields on a command without sub commands") {
      const litr::Config::Query query{config};
      const auto command{query.GetCommand("update")};

      CHECK_EQ(command->Name, "update");
      CHECK_EQ(command->Script.size(), 1);
      CHECK_EQ(command->Script[0], "git pull && git submodule update --init");
      CHECK(command->Description.empty());
      CHECK(command->Example.empty());
      CHECK_EQ(command->Output, litr::Config::Command::Output::UNCHANGED);
      CHECK(command->Directory.empty());
      CHECK(command->ChildCommands.empty());
    }

    SUBCASE("Resolves all fields on a sub command") {
      const litr::Config::Query query{config};
      const auto command1{query.GetCommand("run.first")};
      const auto command2{query.GetCommand("run.second")};
      const auto command3{query.GetCommand("run.third")};
      const auto command4{query.GetCommand("run.fourth")};

      CHECK_NE(command1, nullptr);
      CHECK_NE(command2, nullptr);
      CHECK_NE(command3, nullptr);
      CHECK_NE(command4, nullptr);

      CHECK_EQ(command1->Name, "first");
      CHECK_EQ(command2->Name, "second");
      CHECK_EQ(command3->Name, "third");
      CHECK_EQ(command4->Name, "fourth");

      CHECK_EQ(command1->Script.size(), 1);
      CHECK_EQ(command1->Script[0], "run first");
      CHECK_EQ(command2->Script.size(), 1);
      CHECK_EQ(command2->Script[0], "run second");
      CHECK_EQ(command3->Script.size(), 2);
      CHECK_EQ(command3->Script[0], "Script1");
      CHECK_EQ(command3->Script[1], "Script2");
      CHECK_EQ(command4->Script.size(), 1);
      CHECK_EQ(command4->Script[0], "run fourth");

      CHECK_EQ(command3->Description, "Description");
      CHECK_EQ(command3->Directory.size(), 2);
      CHECK_EQ(command3->Directory[0], "../../Fixtures/Config/Directory1");
      CHECK_EQ(command3->Directory[1], "../../Fixtures/Config/Directory2");
    }

    SUBCASE("Resolves a very deep nested script") {
      const litr::Config::Query query{config};
      const auto command{query.GetCommand("run.fourth.l1.l2.l3")};

      CHECK_NE(command, nullptr);
      CHECK_EQ(command->Name, "l3");
      CHECK_EQ(command->Script.size(), 1);
      CHECK_EQ(command->Script[0], "Deep Script");
    }
  }

  TEST_CASE("Emits an error if parameter name is reserved for Litr") {
    const litr::Path path{"../../Fixtures/Config/reserved-parameter.toml"};
    const litr::Config::Loader config{path};
    const auto errors{litr::Error::Handler::GetErrors()};

    CHECK(litr::Error::Handler::HasErrors());
    CHECK_EQ(errors.size(), 2);
    CHECK_EQ(errors[0].Message, R"(The parameter name "h" is reserved by Litr.)");
    CHECK_EQ(errors[1].Message, R"(The parameter name "help" is reserved by Litr.)");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Emits an error if parameter definition is malformed") {
    const litr::Path path{"../../Fixtures/Config/malformed-parameter.toml"};
    const litr::Config::Loader config{path};
    const auto errors{litr::Error::Handler::GetErrors()};

    CHECK(litr::Error::Handler::HasErrors());
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "A parameter needs to be a string or table.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Copies errors from ParameterBuilder to Loader on multiple malformed params") {
    const litr::Path path{"../../Fixtures/Config/params-description-and-type-malformed.toml"};
    const litr::Config::Loader config{path};
    const auto errors{litr::Error::Handler::GetErrors()};

    CHECK(litr::Error::Handler::HasErrors());
    CHECK_EQ(errors.size(), 2);
    CHECK_EQ(errors[0].Message, R"(The "description" can only be a string.)");
    CHECK_EQ(errors[1].Message, R"(A "type" can only be "string" or an array of options as strings.)");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Loads Parameters") {
    const litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const auto config{litr::CreateRef<litr::Config::Loader>(path)};

    SUBCASE("Successfully without errors") {
      CHECK_FALSE(litr::Error::Handler::HasErrors());
      CHECK_EQ(config->GetParameters().size(), 2);
    }

    SUBCASE("Resolves all fields on a parameter") {
      const litr::Config::Query query{config};
      const auto param{query.GetParameter("target")};

      CHECK_EQ(param->Name, "target");
      CHECK_EQ(param->Description, "Description");
      CHECK_EQ(param->Shortcut, "t");
      CHECK_EQ(param->Default, "debug");
      CHECK_EQ(param->Type, litr::Config::Parameter::Type::ARRAY);
      CHECK_EQ(param->TypeArguments.size(), 2);
      CHECK_EQ(param->TypeArguments[0], "debug");
      CHECK_EQ(param->TypeArguments[1], "release");
    }
  }
}
