#include <doctest/doctest.h>

#include "Core/Errors/ErrorHandler.hpp"
#include "Core/FileSystem.hpp"
#include "Core/Config/ConfigLoader.hpp"

TEST_SUITE("ConfigLoader") {
  TEST_CASE("Loads a config file without issues") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const Litr::Path path{"../../Fixtures/Config/empty.toml"};
    const Litr::ConfigLoader config{errorHandler, path};

    CHECK(errorHandler->HasErrors() == false);
  }

  TEST_CASE("Emits a custom syntax error on exception") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const Litr::Path path{"../../Fixtures/Config/syntax-error.toml"};
    const Litr::ConfigLoader config{errorHandler, path};
    const auto errors{errorHandler->GetErrors()};

    CHECK(errorHandler->HasErrors() == true);
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "There is a syntax error inside the configuration file.");
  }

  TEST_CASE("Emits an error on malformed command") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const Litr::Path path{"../../Fixtures/Config/malformed-command.toml"};
    const Litr::ConfigLoader config{errorHandler, path};
    const auto errors{errorHandler->GetErrors()};

    CHECK(errorHandler->HasErrors() == true);
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "A command can be a string or table.");
  }

  TEST_CASE("Emits an error if command script is not a string") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const Litr::Path path{"../../Fixtures/Config/malformed-command-script.toml"};
    const Litr::ConfigLoader config{errorHandler, path};
    const auto errors{errorHandler->GetErrors()};

    CHECK(errorHandler->HasErrors() == true);
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "A command script can be either a string or array of strings.");
  }

  TEST_CASE("Emits an error if command property unknown") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const Litr::Path path{"../../Fixtures/Config/unknown-command-property.toml"};
    const Litr::ConfigLoader config{errorHandler, path};
    const auto errors{errorHandler->GetErrors()};

    CHECK(errorHandler->HasErrors() == true);
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == R"(The command property "unknown" does not exist. Please refer to the docs.)");
  }

  TEST_CASE("Copies errors from CommandBuilder to ConfigLoader on malformed script array") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const Litr::Path path{"../../Fixtures/Config/command-script-array-malformed.toml"};
    const Litr::ConfigLoader config{errorHandler, path};
    const auto errors{errorHandler->GetErrors()};

    CHECK(errorHandler->HasErrors() == true);
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "A command script can be either a string or array of strings.");
  }

  TEST_CASE("Copies errors from CommandBuilder to ConfigLoader on detailed malformed script array") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const Litr::Path path{"../../Fixtures/Config/command-detailed-script-array-malformed.toml"};
    const Litr::ConfigLoader config{errorHandler, path};
    const auto errors{errorHandler->GetErrors()};

    CHECK(errorHandler->HasErrors() == true);
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "A command script can be either a string or array of strings.");
  }

  TEST_CASE("Copies errors from CommandBuilder to ConfigLoader on multiple malformed fields") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const Litr::Path path{"../../Fixtures/Config/command-description-and-output-malformed.toml"};
    const Litr::ConfigLoader config{errorHandler, path};
    const auto errors{errorHandler->GetErrors()};

    CHECK(errorHandler->HasErrors() == true);
    CHECK(errors.size() == 2);
    CHECK(errors[0].Message == R"(The "description" can can only be a string.)");
    CHECK(errors[1].Message == R"(The "output" can either be "unchanged" or "silent".)");
  }

  TEST_CASE("Does nothing if no commands or parameters defined") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const Litr::Path path{"../../Fixtures/Config/empty-commands-params.toml"};
    const Litr::ConfigLoader config{errorHandler, path};

    CHECK(errorHandler->HasErrors() == false);
    CHECK(config.GetCommands().size() == 0);
    CHECK(config.GetParameters().size() == 0);
  }

  TEST_CASE("Loads commands") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const Litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const Litr::ConfigLoader config{errorHandler, path};

    SUBCASE("Successfully without errors") {
      CHECK(errorHandler->HasErrors() == false);
      CHECK(config.GetCommands().size() == 3);
    }

    SUBCASE("Resolves all fields on a command") {
      const auto command{config.GetCommand("run")};

      CHECK(command->Name == "run");
      CHECK(command->Script.size() == 1);
      CHECK(command->Script[0] == "Script");
      CHECK(command->Description == "Description");
      CHECK(command->Example == "Example");
      CHECK(command->Output == Litr::Command::Output::SILENT);
      CHECK(command->Directory.size() == 1);
      CHECK(command->Directory[0] == "Directory");
      CHECK(command->ChildCommands.size() == 4);
    }

    SUBCASE("Resolves all fields on a command without sub commands") {
      const auto command{config.GetCommand("update")};

      CHECK(command->Name == "update");
      CHECK(command->Script.size() == 1);
      CHECK(command->Script[0] == "git pull && git submodule update --init");
      CHECK(command->Description.empty());
      CHECK(command->Example.empty());
      CHECK(command->Output == Litr::Command::Output::UNCHANGED);
      CHECK(command->Directory.empty());
      CHECK(command->ChildCommands.empty());
    }

    SUBCASE("Resolves all fields on a sub command") {
      const auto command1{config.GetCommand("run.first")};
      const auto command2{config.GetCommand("run.second")};
      const auto command3{config.GetCommand("run.third")};
      const auto command4{config.GetCommand("run.fourth")};

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
      const auto command{config.GetCommand("run.fourth.l1.l2.l3")};

      CHECK(command != nullptr);
      CHECK(command->Name == "l3");
      CHECK(command->Script.size() == 1);
      CHECK(command->Script[0] == "Deep Script");
    }
  }

  TEST_CASE("Emits an error if parameter name is reserved for Litr") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const Litr::Path path{"../../Fixtures/Config/reserved-parameter.toml"};
    const Litr::ConfigLoader config{errorHandler, path};
    const auto errors{errorHandler->GetErrors()};

    CHECK(errorHandler->HasErrors() == true);
    CHECK(errors.size() == 2);
    CHECK(errors[0].Message == R"(The parameter name "h" is reserved by Litr.)");
    CHECK(errors[1].Message == R"(The parameter name "help" is reserved by Litr.)");
  }

  TEST_CASE("Emits an error if parameter definition is malformed") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const Litr::Path path{"../../Fixtures/Config/malformed-parameter.toml"};
    const Litr::ConfigLoader config{errorHandler, path};
    const auto errors{errorHandler->GetErrors()};

    CHECK(errorHandler->HasErrors() == true);
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "A parameter needs to be a string or table.");
  }

  TEST_CASE("Copies errors from ParameterBuilder to ConfigLoader on multiple malformed params") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const Litr::Path path{"../../Fixtures/Config/params-description-and-type-malformed.toml"};
    const Litr::ConfigLoader config{errorHandler, path};
    const auto errors{errorHandler->GetErrors()};

    CHECK(errorHandler->HasErrors() == true);
    CHECK(errors.size() == 2);
    CHECK(errors[0].Message == R"(The "description" can can only be a string.)");
    CHECK(errors[1].Message == R"(A "type" can can only be "string" or an array of options as strings.)");
  }

  TEST_CASE("Loads Parameters") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const Litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const Litr::ConfigLoader config{errorHandler, path};

    SUBCASE("Successfully without errors") {
      CHECK(errorHandler->HasErrors() == false);
      CHECK(config.GetParameters().size() == 2);
    }

    SUBCASE("Resolves all fields on a parameter") {
      const auto param{config.GetParameter("target")};

      CHECK(param->Name == "target");
      CHECK(param->Description == "Description");
      CHECK(param->Shortcut == "t");
      CHECK(param->Default == "debug");
      CHECK(param->Type == Litr::Parameter::ParameterType::Array);
      CHECK(param->TypeArguments.size() == 2);
      CHECK(param->TypeArguments[0] == "debug");
      CHECK(param->TypeArguments[1] == "release");
    }
  }
}
