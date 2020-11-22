#include <doctest/doctest.h>

#include "Core/FileSystem.hpp"
#include "Core/Config/ConfigLoader.hpp"

TEST_SUITE("ConfigLoader") {
  TEST_CASE("Loads a config file without issues") {
    const Litr::Path path{"./Fixtures/empty-config.toml"};
    const Litr::ConfigLoader config{path};

    CHECK(config.HasErrors() == false);
  }

  TEST_CASE("Emits a custom syntax error on exception") {
    const Litr::Path path{"./Fixtures/syntax-error-config.toml"};
    const Litr::ConfigLoader config{path};
    const auto errors{config.GetErrors()};

    CHECK(config.HasErrors() == true);
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "There is a syntax error inside the configuration file.");
  }

  TEST_CASE("Does nothing if no commands defined") {
    const Litr::Path path{"./Fixtures/empty-commands-params-config.toml"};
    const Litr::ConfigLoader config{path};

    CHECK(config.HasErrors() == false);
    CHECK(config.GetCommands().size() == 0);
  }

  TEST_CASE("Loads commands") {
    const Litr::Path path{"./Fixtures/commands-params-config.toml"};
    const Litr::ConfigLoader config{path};

    SUBCASE("Successfully without errors") {
      CHECK(config.HasErrors() == false);
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
}
