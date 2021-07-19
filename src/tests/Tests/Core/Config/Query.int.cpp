#include <doctest/doctest.h>

#include "Core/Config/Query.hpp"

TEST_SUITE("Config::Query") {
  TEST_CASE("GetCommand()") {
    const Litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const auto config{Litr::CreateRef<Litr::Config::Loader>(path)};
    const Litr::Config::Query query{config};

    SUBCASE("Get top level command") {
      CHECK(query.GetCommand("run")->Name == "run");
    }

    SUBCASE("Get nested command") {
      CHECK(query.GetCommand("run.second")->Name == "second");
    }

    SUBCASE("Get deeply nested command") {
      CHECK(query.GetCommand("run.fourth.l1")->Name == "l1");
    }

    SUBCASE("Returns a nullptr if command not found") {
      CHECK(query.GetCommand("nothing") == nullptr);
    }

    SUBCASE("Returns a nullptr if nested command not found") {
      CHECK(query.GetCommand("run.somewhere") == nullptr);
    }
  }

  TEST_CASE("GetCommands()") {
    const Litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const auto config{Litr::CreateRef<Litr::Config::Loader>(path)};
    const Litr::Config::Query query{config};

    SUBCASE("Get all defined commands") {
      CHECK(query.GetCommands().size() == 3);
    }

    SUBCASE("Get all defined sub-commands") {
      CHECK(query.GetCommands("run").size() == 4);
    }

    SUBCASE("Returns an empty vector for undefined script") {
      CHECK(query.GetCommands("none").size() == 0);
    }
  }

  TEST_CASE("GetParameter()") {
    const Litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const auto config{Litr::CreateRef<Litr::Config::Loader>(path)};
    const Litr::Config::Query query{config};

    SUBCASE("Get parameter by name") {
      CHECK(query.GetParameter("target")->Name == "target");
    }

    SUBCASE("Get parameter by short name") {
      CHECK(query.GetParameter("t")->Name == "target");
    }

    SUBCASE("Returns a nullptr for an undefined parameter name") {
      CHECK(query.GetParameter("x") == nullptr);
    }
  }

  TEST_CASE("GetParameters()") {
    const Litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const auto config{Litr::CreateRef<Litr::Config::Loader>(path)};
    const Litr::Config::Query query{config};

    SUBCASE("Get all defined parameters") {
      CHECK(query.GetParameters().size() == 2);
    }

    SUBCASE("Returns an empty vector for undefined script") {
      auto params{query.GetParameters("none")};
      CHECK(params.size() == 0);
    }

    // @todo: Figure out why this fails in CI
    // "ERROR: test case THREW exception: Unexpected index"
    // https://github.com/krieselreihe/litr/issues/42
//    SUBCASE("Get parameters used by command") {
//      auto params{query.GetParameters("build")};
//      CHECK(params.size() == 1);
//      CHECK(params[0]->Name == "target");
//    }

    SUBCASE("Get no parameters if none are used by command") {
      auto params{query.GetParameters("update")};
      CHECK(params.size() == 0);
    }

    SUBCASE("Returns an empty vector if command not found") {
      auto params{query.GetParameters("none")};
      CHECK(params.size() == 0);
    }
  }
}
