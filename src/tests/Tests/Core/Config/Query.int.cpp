#include <doctest/doctest.h>

#include "Core/Config/Query.hpp"

TEST_SUITE("Config::Query") {
  TEST_CASE("GetCommand()") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const Litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const auto config{Litr::CreateRef<Litr::Config::Loader>(errorHandler, path)};
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

  TEST_CASE("GetParameter()") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const Litr::Path path{"../../Fixtures/Config/commands-params.toml"};
    const auto config{Litr::CreateRef<Litr::Config::Loader>(errorHandler, path)};
    const Litr::Config::Query query{config};

    SUBCASE("Get parameter by name") {
      CHECK(query.GetParameter("target")->Name == "target");
    }

    SUBCASE("Get parameter by short name") {
      CHECK(query.GetParameter("t")->Name == "target");
    }
  }
}
