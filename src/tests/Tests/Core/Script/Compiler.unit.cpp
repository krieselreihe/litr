/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/Script/Compiler.hpp"

#include <doctest/doctest.h>

#include <string>

#include "Core/Config/Location.hpp"
#include "Core/Error/Handler.hpp"

TEST_SUITE("Script::Compiler") {
  using Variables = Litr::Script::Compiler::Variables;
  using Location = Litr::Config::Location;
  using Compiler = Litr::Script::Compiler;

  // Success
  TEST_CASE("Single string") {
    const std::string source{"echo '%{'Hello'}'"};
    const Location location{1, 1, R"(script = "echo '%{'Hello'}'")"};
    const Variables variables{};
    const Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.result(), "echo 'Hello'");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Single string in the middle") {
    const std::string source{"echo '%{'Hello'}' and more"};
    const Location location{1, 1, R"(script = "echo '%{'Hello'}' and more")"};
    const Variables variables{};
    const Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.result(), "echo 'Hello' and more");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Single variable") {
    const std::string source{"echo '%{target}'"};
    const Location location{1, 1, R"(script = "echo '%{target}'")"};
    const Variables variables{{{"target", Litr::CLI::Variable("target", std::string("Hello"))}}};
    const Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.result(), "echo 'Hello'");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Single variable in the middle") {
    const std::string source{"echo '%{target}' and more"};
    const Location location{1, 1, R"(script = "echo '%{target}' and more")"};
    const Variables variables{{{"target", Litr::CLI::Variable("target", std::string("Hello"))}}};
    const Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.result(), "echo 'Hello' and more");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Single true boolean variable") {
    const std::string source{"echo %{target 'Hello'}"};
    const Location location{1, 1, R"(script = "echo %{target 'Hello'}")"};
    const Variables variables{{{"target", Litr::CLI::Variable("target", true)}}};
    const Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.result(), "echo Hello");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Single false boolean variable") {
    const std::string source{"echo %{target 'Hello'}"};
    const Location location{1, 1, R"(script = "echo %{target 'Hello'}")"};
    const Variables variables{{"target", Litr::CLI::Variable("target", false)}};
    const Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.result(), "echo ");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Boolean variable printing value of second variable") {
    const std::string source{"echo '%{target value}'"};
    const Location location{1, 1, R"(script = "echo '%{target value}'")"};
    const Variables variables{{{"target", Litr::CLI::Variable("target", true)},
        {"value", Litr::CLI::Variable("value", std::string("Hello"))}}};
    const Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.result(), "echo 'Hello'");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Boolean variable not printing value of second variable") {
    const std::string source{"echo '%{target value}'"};
    const Location location{1, 1, R"(script = "echo '%{target value}'")"};
    const Variables variables{{{"target", Litr::CLI::Variable("target", false)},
        {"value", Litr::CLI::Variable("value", std::string("Hello"))}}};
    const Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.result(), "echo ''");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Variable with true or-statement") {
    const std::string source{"echo %{target 'Hello' or 'Bye'}"};
    const Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    const Variables variables{{{"target", Litr::CLI::Variable("target", true)}}};
    const Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.result(), "echo Hello");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Variable with false or-statement") {
    const std::string source{R"(echo %{target 'Hello' or 'Bye'})"};
    const Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    const Variables variables{{{"target", Litr::CLI::Variable("target", false)}}};
    const Compiler compiler{source, location, variables};

    CHECK_FALSE(Litr::Error::Handler::has_errors());
    CHECK_EQ(compiler.result(), "echo Bye");
    Litr::Error::Handler::flush();
  }

  // Error

  TEST_CASE("Variable with undefined variable or-statement") {
    const std::string source{R"(echo %{target 'Hello' or 'Bye'})"};
    const Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    const Variables variables{};
    const Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `target`: Undefined parameter.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Nested true statements will fail") {
    const std::string source{R"(echo %{a b 'Wrong' 'Hello' or 'Bye'})"};
    const Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    const Variables variables{
        {"a", Litr::CLI::Variable("a", true)}, {"b", Litr::CLI::Variable("b", true)}};
    const Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'Hello'`: Expected `}`.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Nested false and true statements will fail") {
    const std::string source{R"(echo %{a b 'Wrong' 'Hello' or 'Bye'})"};
    const Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    const Variables variables{
        {{"a", Litr::CLI::Variable("a", true)}, {"b", Litr::CLI::Variable("b", false)}}};
    const Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'Hello'`: Expected `}`.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Nested true and false statements will fail") {
    const std::string source{R"(echo %{a b 'Wrong' 'Hello' or 'Bye'})"};
    const Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    const Variables variables{
        {{"a", Litr::CLI::Variable("a", false)}, {"b", Litr::CLI::Variable("b", true)}}};
    const Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'Wrong'`: Expected `}`.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Nested false statements will fail") {
    const std::string source{R"(echo %{a b 'Wrong' 'Hello' or 'Bye'})"};
    const Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    const Variables variables{
        {{"a", Litr::CLI::Variable("a", false)}, {"b", Litr::CLI::Variable("b", false)}}};
    const Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'Wrong'`: Expected `}`.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Multiple strings in a row fail") {
    const std::string source{"echo '%{'Hello' ' ' 'World'}'"};
    const Location location{1, 1, R"(script = "echo '%{'Hello' ' ' 'World'}'")"};
    const Variables variables{};
    const Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `' '`: Expected `}`.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Does throw on multiple non boolean variables in a row") {
    const std::string source{"echo '%{target value}'"};
    const Location location{1, 1, R"(script = "echo '%{target value}'")"};
    const Variables variables{{{"target", Litr::CLI::Variable("target", std::string("Hello"))},
        {"value", Litr::CLI::Variable("value", std::string(" World"))}}};
    const Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `value`: Expected `}`.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Fails on duplicated 'or' for true case") {
    const std::string source{"echo '%{target 'a' or or 'b'}'"};
    const Location location{1, 1, R"(script = "echo '%{target 'a' or or 'b'}'")"};
    const Variables variables{{{"target", Litr::CLI::Variable("target", true)}}};
    const Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'b'`: Expected `}`.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Fails on duplicated 'or' for false case") {
    const std::string source{"echo '%{target 'a' or or 'b'}'"};
    const Location location{1, 1, R"(script = "echo '%{target 'a' or or 'b'}'")"};
    const Variables variables{{{"target", Litr::CLI::Variable("target", false)}}};
    const Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `'b'`: Expected `}`.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Failed on single or") {
    const std::string source{"echo %{or}"};
    const Location location{1, 1, R"(script = "echo %{or}")"};
    const Variables variables{};
    const Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `or`: Unexpected character.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Failed on single comma") {
    const std::string source{"echo %{,}"};
    const Location location{1, 1, R"(script = "echo %{,}")"};
    const Variables variables{};
    const Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Unexpected character.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Failed on standalone parens") {
    const std::string source{"echo %{()}"};
    const Location location{1, 1, R"(script = "echo %{()}")"};
    const Variables variables{};
    const Compiler compiler{source, location, variables};

    CHECK(Litr::Error::Handler::has_errors());

    auto errors{Litr::Error::Handler::errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `(`: Unexpected character.");
    Litr::Error::Handler::flush();
  }
}
