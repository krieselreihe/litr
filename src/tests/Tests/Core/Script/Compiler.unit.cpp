/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/Script/Compiler.hpp"

#include <doctest/doctest.h>

#include <string>

#include "Core/Config/Location.hpp"
#include "Core/Error/Handler.hpp"

TEST_SUITE("script::Compiler") {
  using Variables = litr::script::Compiler::Variables;
  using Location = litr::config::Location;
  using Compiler = litr::script::Compiler;

  // Success
  TEST_CASE("Single string") {
    const std::string source{"echo '%{'Hello'}'"};
    Location location{1, 1, R"(script = "echo '%{'Hello'}'")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo 'Hello'");
    litr::error::Handler::flush();
  }

  TEST_CASE("Single string in the middle") {
    const std::string source{"echo '%{'Hello'}' and more"};
    Location location{1, 1, R"(script = "echo '%{'Hello'}' and more")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo 'Hello' and more");
    litr::error::Handler::flush();
  }

  TEST_CASE("Single variable") {
    const std::string source{"echo '%{target}'"};
    Location location{1, 1, R"(script = "echo '%{target}'")"};
    Variables variables{{{"target", litr::cli::Variable("target", std::string("Hello"))}}};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo 'Hello'");
    litr::error::Handler::flush();
  }

  TEST_CASE("Single variable in the middle") {
    const std::string source{"echo '%{target}' and more"};
    Location location{1, 1, R"(script = "echo '%{target}' and more")"};
    Variables variables{{{"target", litr::cli::Variable("target", std::string("Hello"))}}};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo 'Hello' and more");
    litr::error::Handler::flush();
  }

  TEST_CASE("Single true boolean variable") {
    const std::string source{"echo %{target 'Hello'}"};
    Location location{1, 1, R"(script = "echo %{target 'Hello'}")"};
    Variables variables{{{"target", litr::cli::Variable("target", true)}}};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo Hello");
    litr::error::Handler::flush();
  }

  TEST_CASE("Single false boolean variable") {
    const std::string source{"echo %{target 'Hello'}"};
    Location location{1, 1, R"(script = "echo %{target 'Hello'}")"};
    Variables variables{{"target", litr::cli::Variable("target", false)}};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo ");
    litr::error::Handler::flush();
  }

  TEST_CASE("Boolean variable printing value of second variable") {
    const std::string source{"echo '%{target value}'"};
    Location location{1, 1, R"(script = "echo '%{target value}'")"};
    Variables variables{{{"target", litr::cli::Variable("target", true)},
        {"value", litr::cli::Variable("value", std::string("Hello"))}}};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo 'Hello'");
    litr::error::Handler::flush();
  }

  TEST_CASE("Boolean variable not printing value of second variable") {
    const std::string source{"echo '%{target value}'"};
    Location location{1, 1, R"(script = "echo '%{target value}'")"};
    Variables variables{{{"target", litr::cli::Variable("target", false)},
        {"value", litr::cli::Variable("value", std::string("Hello"))}}};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo ''");
    litr::error::Handler::flush();
  }

  TEST_CASE("Variable with true or-statement") {
    const std::string source{"echo %{target 'Hello' or 'Bye'}"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{{{"target", litr::cli::Variable("target", true)}}};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo Hello");
    litr::error::Handler::flush();
  }

  TEST_CASE("Variable with false or-statement") {
    const std::string source{R"(echo %{target 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{{{"target", litr::cli::Variable("target", false)}}};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo Bye");
    litr::error::Handler::flush();
  }

  // Error

  TEST_CASE("Variable with undefined variable or-statement") {
    const std::string source{R"(echo %{target 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK(litr::error::Handler::has_errors());

    auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `target`: Undefined parameter.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Nested true statements will fail") {
    const std::string source{R"(echo %{a b 'Wrong' 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{
        {"a", litr::cli::Variable("a", true)}, {"b", litr::cli::Variable("b", true)}};
    Compiler compiler{source, location, variables};

    CHECK(litr::error::Handler::has_errors());

    auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'Hello'`: Expected `}`.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Nested false and true statements will fail") {
    const std::string source{R"(echo %{a b 'Wrong' 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{
        {{"a", litr::cli::Variable("a", true)}, {"b", litr::cli::Variable("b", false)}}};
    Compiler compiler{source, location, variables};

    CHECK(litr::error::Handler::has_errors());

    auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'Hello'`: Expected `}`.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Nested true and false statements will fail") {
    const std::string source{R"(echo %{a b 'Wrong' 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{
        {{"a", litr::cli::Variable("a", false)}, {"b", litr::cli::Variable("b", true)}}};
    Compiler compiler{source, location, variables};

    CHECK(litr::error::Handler::has_errors());

    auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'Wrong'`: Expected `}`.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Nested false statements will fail") {
    const std::string source{R"(echo %{a b 'Wrong' 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{
        {{"a", litr::cli::Variable("a", false)}, {"b", litr::cli::Variable("b", false)}}};
    Compiler compiler{source, location, variables};

    CHECK(litr::error::Handler::has_errors());

    auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'Wrong'`: Expected `}`.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Multiple strings in a row fail") {
    const std::string source{"echo '%{'Hello' ' ' 'World'}'"};
    Location location{1, 1, R"(script = "echo '%{'Hello' ' ' 'World'}'")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK(litr::error::Handler::has_errors());

    auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `' '`: Expected `}`.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Does throw on multiple non boolean variables in a row") {
    const std::string source{"echo '%{target value}'"};
    Location location{1, 1, R"(script = "echo '%{target value}'")"};
    Variables variables{{{"target", litr::cli::Variable("target", std::string("Hello"))},
        {"value", litr::cli::Variable("value", std::string(" World"))}}};
    Compiler compiler{source, location, variables};

    CHECK(litr::error::Handler::has_errors());

    auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `value`: Expected `}`.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Fails on duplicated 'or' for true case") {
    const std::string source{"echo '%{target 'a' or or 'b'}'"};
    Location location{1, 1, R"(script = "echo '%{target 'a' or or 'b'}'")"};
    Variables variables{{{"target", litr::cli::Variable("target", true)}}};
    Compiler compiler{source, location, variables};

    CHECK(litr::error::Handler::has_errors());

    auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'b'`: Expected `}`.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Fails on duplicated 'or' for false case") {
    const std::string source{"echo '%{target 'a' or or 'b'}'"};
    Location location{1, 1, R"(script = "echo '%{target 'a' or or 'b'}'")"};
    Variables variables{{{"target", litr::cli::Variable("target", false)}}};
    Compiler compiler{source, location, variables};

    CHECK(litr::error::Handler::has_errors());

    auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'b'`: Expected `}`.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Failed on single or") {
    const std::string source{"echo %{or}"};
    Location location{1, 1, R"(script = "echo %{or}")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK(litr::error::Handler::has_errors());

    auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `or`: Unexpected character.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Failed on single comma") {
    const std::string source{"echo %{,}"};
    Location location{1, 1, R"(script = "echo %{,}")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK(litr::error::Handler::has_errors());

    auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Unexpected character.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Failed on standalone parens") {
    const std::string source{"echo %{()}"};
    Location location{1, 1, R"(script = "echo %{()}")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK(litr::error::Handler::has_errors());

    auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `(`: Unexpected character.");
    litr::error::Handler::flush();
  }
}
