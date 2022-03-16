/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/Config/Query.hpp"

#include <doctest/doctest.h>

#include <memory>

TEST_SUITE("Config::Query") {
  TEST_CASE("get_command()") {
    const litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const auto config{std::make_shared<litr::config::Loader>(path)};
    const litr::config::Query query{config};

    SUBCASE("Get top level command") {
      CHECK_EQ(query.get_command("run")->name, "run");
    }

    SUBCASE("Get nested command") {
      CHECK_EQ(query.get_command("run.second")->name, "second");
    }

    SUBCASE("Get deeply nested command") {
      CHECK_EQ(query.get_command("run.fourth.l1")->name, "l1");
    }

    SUBCASE("Returns a nullptr if command not found") {
      CHECK_EQ(query.get_command("nothing"), nullptr);
    }

    SUBCASE("Returns a nullptr if nested command not found") {
      CHECK_EQ(query.get_command("run.somewhere"), nullptr);
    }
  }

  TEST_CASE("get_commands()") {
    const litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const auto config{std::make_shared<litr::config::Loader>(path)};
    const litr::config::Query query{config};

    SUBCASE("Get all defined commands") {
      CHECK_EQ(query.get_commands().size(), 3);
    }

    SUBCASE("Get all defined sub-commands") {
      CHECK_EQ(query.get_commands("run").size(), 4);
    }

    SUBCASE("Returns an empty vector for undefined script") {
      CHECK_EQ(query.get_commands("none").size(), 0);
    }
  }

  TEST_CASE("get_parameter()") {
    const litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const auto config{std::make_shared<litr::config::Loader>(path)};
    const litr::config::Query query{config};

    SUBCASE("Get parameter by name") {
      CHECK_EQ(query.get_parameter("target")->name, "target");
    }

    SUBCASE("Get parameter by short name") {
      CHECK_EQ(query.get_parameter("t")->name, "target");
    }

    SUBCASE("Returns a nullptr for an undefined parameter name") {
      CHECK_EQ(query.get_parameter("x"), nullptr);
    }
  }

  TEST_CASE("get_parameters()") {
    const litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const auto config{std::make_shared<litr::config::Loader>(path)};
    const litr::config::Query query{config};

    SUBCASE("Get all defined parameters") {
      CHECK_EQ(query.get_parameters().size(), 2);
    }

    SUBCASE("Returns an empty vector for undefined script") {
      auto params{query.get_parameters("none")};
      CHECK_EQ(params.size(), 0);
    }

    // @todo: Figure out why this fails in CI. Maybe related to the used
    // version of gcc.
    // "ERROR: test case THREW exception: Unexpected index"
    // https://github.com/krieselreihe/litr/issues/42
    //    SUBCASE("Get parameters used by command") {
    //      auto params{query.GetParameters("build")};
    //      CHECK_EQ(params.size(), 1);
    //      CHECK_EQ(params[0]->Name, "target");
    //    }

    SUBCASE("Get no parameters if none are used by command") {
      auto params{query.get_parameters("update")};
      CHECK_EQ(params.size(), 0);
    }

    SUBCASE("Returns an empty vector if command not found") {
      auto params{query.get_parameters("none")};
      CHECK_EQ(params.size(), 0);
    }
  }
}
