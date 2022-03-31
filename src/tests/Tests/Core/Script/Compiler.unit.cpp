/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/Script/Compiler.hpp"

#include <doctest/doctest.h>

#include <string>

#include "Core/Config/Location.hpp"
#include "Core/Error/Handler.hpp"

TEST_SUITE("script::Compiler") {
  using Variables = Litr::Script::Compiler::Variables;
  using Location = Litr::Config::Location;
  using Compiler = Litr::Script::Compiler;

  // Success
  TEST_CASE("Single string") {
    const std::string source{"echo '%{'Hello'}'"};
    Location location{1, 1, R"(script = "echo '%{'Hello'}'")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo 'Hello'");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Single string in the middle") {
    const std::string source{"echo '%{'Hello'}' and more"};
    Location location{1, 1, R"(script = "echo '%{'Hello'}' and more")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo 'Hello' and more");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Single variable") {
    const std::string source{"echo '%{target}'"};
    Location location{1, 1, R"(script = "echo '%{target}'")"};
    Variables variables{{{"target", Litr::CLI::Variable("target", std::string("Hello"))}}};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo 'Hello'");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Single variable in the middle") {
    const std::string source{"echo '%{target}' and more"};
    Location location{1, 1, R"(script = "echo '%{target}' and more")"};
    Variables variables{{{"target", Litr::CLI::Variable("target", std::string("Hello"))}}};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo 'Hello' and more");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Single true boolean variable") {
    const std::string source{"echo %{target 'Hello'}"};
    Location location{1, 1, R"(script = "echo %{target 'Hello'}")"};
    Variables variables{{{"target", Litr::CLI::Variable("target", true)}}};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo Hello");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Single false boolean variable") {
    const std::string source{"echo %{target 'Hello'}"};
    Location location{1, 1, R"(script = "echo %{target 'Hello'}")"};
    Variables variables{{"target", Litr::CLI::Variable("target", false)}};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo ");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Boolean variable printing value of second variable") {
    const std::string source{"echo '%{target value}'"};
    Location location{1, 1, R"(script = "echo '%{target value}'")"};
    Variables variables{{{"target", Litr::CLI::Variable("target", true)},
        {"value", Litr::CLI::Variable("value", std::string("Hello"))}}};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo 'Hello'");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Boolean variable not printing value of second variable") {
    const std::string source{"echo '%{target value}'"};
    Location location{1, 1, R"(script = "echo '%{target value}'")"};
    Variables variables{{{"target", Litr::CLI::Variable("target", false)},
        {"value", Litr::CLI::Variable("value", std::string("Hello"))}}};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo ''");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Variable with true or-statement") {
    const std::string source{"echo %{target 'Hello' or 'Bye'}"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{{{"target", Litr::CLI::Variable("target", true)}}};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo Hello");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Variable with false or-statement") {
    const std::string source{R"(echo %{target 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{{{"target", Litr::CLI::Variable("target", false)}}};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.get_script(), "echo Bye");
    Litr::Error::Handler::flush();
  }

  // Error

  TEST_CASE("Variable with undefined variable or-statement") {
    const std::string source{R"(echo %{target 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `target`: Undefined parameter.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Nested true statements will fail") {
    const std::string source{R"(echo %{a b 'Wrong' 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{
        {"a", Litr::CLI::Variable("a", true)}, {"b", Litr::CLI::Variable("b", true)}};
    Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'Hello'`: Expected `}`.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Nested false and true statements will fail") {
    const std::string source{R"(echo %{a b 'Wrong' 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{
        {{"a", Litr::CLI::Variable("a", true)}, {"b", Litr::CLI::Variable("b", false)}}};
    Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'Hello'`: Expected `}`.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Nested true and false statements will fail") {
    const std::string source{R"(echo %{a b 'Wrong' 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{
        {{"a", Litr::CLI::Variable("a", false)}, {"b", Litr::CLI::Variable("b", true)}}};
    Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'Wrong'`: Expected `}`.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Nested false statements will fail") {
    const std::string source{R"(echo %{a b 'Wrong' 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{
        {{"a", Litr::CLI::Variable("a", false)}, {"b", Litr::CLI::Variable("b", false)}}};
    Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'Wrong'`: Expected `}`.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Multiple strings in a row fail") {
    const std::string source{"echo '%{'Hello' ' ' 'World'}'"};
    Location location{1, 1, R"(script = "echo '%{'Hello' ' ' 'World'}'")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `' '`: Expected `}`.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Does throw on multiple non boolean variables in a row") {
    const std::string source{"echo '%{target value}'"};
    Location location{1, 1, R"(script = "echo '%{target value}'")"};
    Variables variables{{{"target", Litr::CLI::Variable("target", std::string("Hello"))},
        {"value", Litr::CLI::Variable("value", std::string(" World"))}}};
    Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `value`: Expected `}`.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Fails on duplicated 'or' for true case") {
    const std::string source{"echo '%{target 'a' or or 'b'}'"};
    Location location{1, 1, R"(script = "echo '%{target 'a' or or 'b'}'")"};
    Variables variables{{{"target", Litr::CLI::Variable("target", true)}}};
    Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'b'`: Expected `}`.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Fails on duplicated 'or' for false case") {
    const std::string source{"echo '%{target 'a' or or 'b'}'"};
    Location location{1, 1, R"(script = "echo '%{target 'a' or or 'b'}'")"};
    Variables variables{{{"target", Litr::CLI::Variable("target", false)}}};
    Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'b'`: Expected `}`.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Failed on single or") {
    const std::string source{"echo %{or}"};
    Location location{1, 1, R"(script = "echo %{or}")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `or`: Unexpected character.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Failed on single comma") {
    const std::string source{"echo %{,}"};
    Location location{1, 1, R"(script = "echo %{,}")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Unexpected character.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Failed on standalone parens") {
    const std::string source{"echo %{()}"};
    Location location{1, 1, R"(script = "echo %{()}")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `(`: Unexpected character.");
    Litr::Error::Handler::flush();
  }
}
