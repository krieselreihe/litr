#include "Core/Config/Loader.hpp"

#include <doctest/doctest.h>

#include "Core/Config/Query.hpp"
#include "Core/Error/Handler.hpp"
#include "Core/FileSystem.hpp"

TEST_SUITE("Config::Loader") {
  TEST_CASE("Loads a config file without issues") {
    const Litr::Path path{"../../Fixtures/Config/empty.toml"};
    const Litr::Config::Loader config{path};

    CHECK(Litr::Error::Handler::HasErrors() == false);
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Emits a custom syntax error on exception") {
    const Litr::Path path{"../../Fixtures/Config/syntax-error.toml"};
    const Litr::Config::Loader config{path};
    const auto errors{Litr::Error::Handler::GetErrors()};

    CHECK(Litr::Error::Handler::HasErrors() == true);
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "There is a syntax error inside the configuration file.");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Emits an error on malformed command") {
    const Litr::Path path{"../../Fixtures/Config/malformed-command.toml"};
    const Litr::Config::Loader config{path};
    const auto errors{Litr::Error::Handler::GetErrors()};

    CHECK(Litr::Error::Handler::HasErrors() == true);
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "A command can be a string or table.");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Emits an error if command script is not a string") {
    const Litr::Path path{"../../Fixtures/Config/malformed-command-script.toml"};
    const Litr::Config::Loader config{path};
    const auto errors{Litr::Error::Handler::GetErrors()};

    CHECK(Litr::Error::Handler::HasErrors() == true);
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "A command script can be either a string or array of strings.");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Emits an error if command property unknown") {
    const Litr::Path path{"../../Fixtures/Config/unknown-command-property.toml"};
    const Litr::Config::Loader config{path};
    const auto errors{Litr::Error::Handler::GetErrors()};

    CHECK(Litr::Error::Handler::HasErrors() == true);
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == R"(The command property "unknown" does not exist. Please refer to the docs.)");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Copies errors from CommandBuilder to Loader on malformed script array") {
    const Litr::Path path{"../../Fixtures/Config/command-script-array-malformed.toml"};
    const Litr::Config::Loader config{path};
    const auto errors{Litr::Error::Handler::GetErrors()};

    CHECK(Litr::Error::Handler::HasErrors() == true);
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "A command script can be either a string or array of strings.");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Copies errors from CommandBuilder to Loader on detailed malformed script array") {
    const Litr::Path path{"../../Fixtures/Config/command-detailed-script-array-malformed.toml"};
    const Litr::Config::Loader config{path};
    const auto errors{Litr::Error::Handler::GetErrors()};

    CHECK(Litr::Error::Handler::HasErrors() == true);
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "A command script can be either a string or array of strings.");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Copies errors from CommandBuilder to Loader on multiple malformed fields") {
    const Litr::Path path{"../../Fixtures/Config/command-description-and-output-malformed.toml"};
    const Litr::Config::Loader config{path};
    const auto errors{Litr::Error::Handler::GetErrors()};

    CHECK(Litr::Error::Handler::HasErrors() == true);
    CHECK(errors.size() == 2);
    CHECK(errors[0].Message == R"(The "description" can can only be a string.)");
    CHECK(errors[1].Message == R"(The "output" can either be "unchanged" or "silent".)");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Does nothing if no commands or parameters defined") {
    const Litr::Path path{"../../Fixtures/Config/empty-commands-params.toml"};
    const Litr::Config::Loader config{path};

    CHECK(Litr::Error::Handler::HasErrors() == false);
    CHECK(config.GetCommands().size() == 0);
    CHECK(config.GetParameters().size() == 0);
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Loads commands") {
    const Litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const auto config{Litr::CreateRef<Litr::Config::Loader>(path)};

    SUBCASE("Successfully without errors") {
      CHECK(Litr::Error::Handler::HasErrors() == false);
      CHECK(config->GetCommands().size() == 3);
    }

    SUBCASE("Resolves all fields on a command") {
      const Litr::Config::Query query{config};
      const auto command{query.GetCommand("run")};

      CHECK(command->Name == "run");
      CHECK(command->Script.size() == 1);
      CHECK(command->Script[0] == "Script");
      CHECK(command->Description == "Description");
      CHECK(command->Example == "Example");
      CHECK(command->Output == Litr::Config::Command::Output::SILENT);
      CHECK(command->Directory.size() == 1);
      CHECK(command->Directory[0] == "Directory");
      CHECK(command->ChildCommands.size() == 4);
    }

    SUBCASE("Resolves all fields on a command without sub commands") {
      const Litr::Config::Query query{config};
      const auto command{query.GetCommand("update")};

      CHECK(command->Name == "update");
      CHECK(command->Script.size() == 1);
      CHECK(command->Script[0] == "git pull && git submodule update --init");
      CHECK(command->Description.empty());
      CHECK(command->Example.empty());
      CHECK(command->Output == Litr::Config::Command::Output::UNCHANGED);
      CHECK(command->Directory.empty());
      CHECK(command->ChildCommands.empty());
    }

    SUBCASE("Resolves all fields on a sub command") {
      const Litr::Config::Query query{config};
      const auto command1{query.GetCommand("run.first")};
      const auto command2{query.GetCommand("run.second")};
      const auto command3{query.GetCommand("run.third")};
      const auto command4{query.GetCommand("run.fourth")};

      CHECK(command1 != nullptr);
      CHECK(command2 != nullptr);
      CHECK(command3 != nullptr);
      CHECK(command4 != nullptr);

      CHECK(command1->Name == "first");
      CHECK(command2->Name == "second");
      CHECK(command3->Name == "third");
      CHECK(command4->Name == "fourth");

      CHECK(command1->Script.size() == 1);
      CHECK(command1->Script[0] == "run first");
      CHECK(command2->Script.size() == 1);
      CHECK(command2->Script[0] == "run second");
      CHECK(command3->Script.size() == 2);
      CHECK(command3->Script[0] == "Script1");
      CHECK(command3->Script[1] == "Script2");
      CHECK(command4->Script.size() == 1);
      CHECK(command4->Script[0] == "run fourth");

      CHECK(command3->Description == "Description");
      CHECK(command3->Directory.size() == 2);
      CHECK(command3->Directory[0] == "Directory1");
      CHECK(command3->Directory[1] == "Directory2");
    }

    SUBCASE("Resolves a very deep nested script") {
      const Litr::Config::Query query{config};
      const auto command{query.GetCommand("run.fourth.l1.l2.l3")};

      CHECK(command != nullptr);
      CHECK(command->Name == "l3");
      CHECK(command->Script.size() == 1);
      CHECK(command->Script[0] == "Deep Script");
    }
  }

  TEST_CASE("Emits an error if parameter name is reserved for Litr") {
    const Litr::Path path{"../../Fixtures/Config/reserved-parameter.toml"};
    const Litr::Config::Loader config{path};
    const auto errors{Litr::Error::Handler::GetErrors()};

    CHECK(Litr::Error::Handler::HasErrors() == true);
    CHECK(errors.size() == 2);
    CHECK(errors[0].Message == R"(The parameter name "h" is reserved by Litr.)");
    CHECK(errors[1].Message == R"(The parameter name "help" is reserved by Litr.)");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Emits an error if parameter definition is malformed") {
    const Litr::Path path{"../../Fixtures/Config/malformed-parameter.toml"};
    const Litr::Config::Loader config{path};
    const auto errors{Litr::Error::Handler::GetErrors()};

    CHECK(Litr::Error::Handler::HasErrors() == true);
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "A parameter needs to be a string or table.");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Copies errors from ParameterBuilder to Loader on multiple malformed params") {
    const Litr::Path path{"../../Fixtures/Config/params-description-and-type-malformed.toml"};
    const Litr::Config::Loader config{path};
    const auto errors{Litr::Error::Handler::GetErrors()};

    CHECK(Litr::Error::Handler::HasErrors() == true);
    CHECK(errors.size() == 2);
    CHECK(errors[0].Message == R"(The "description" can can only be a string.)");
    CHECK(errors[1].Message == R"(A "type" can can only be "string" or an array of options as strings.)");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Loads Parameters") {
    const Litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const auto config{Litr::CreateRef<Litr::Config::Loader>(path)};

    SUBCASE("Successfully without errors") {
      CHECK(Litr::Error::Handler::HasErrors() == false);
      CHECK(config->GetParameters().size() == 2);
    }

    SUBCASE("Resolves all fields on a parameter") {
      const Litr::Config::Query query{config};
      const auto param{query.GetParameter("target")};

      CHECK(param->Name == "target");
      CHECK(param->Description == "Description");
      CHECK(param->Shortcut == "t");
      CHECK(param->Default == "debug");
      CHECK(param->Type == Litr::Config::Parameter::Type::ARRAY);
      CHECK(param->TypeArguments.size() == 2);
      CHECK(param->TypeArguments[0] == "debug");
      CHECK(param->TypeArguments[1] == "release");
    }
  }
}
