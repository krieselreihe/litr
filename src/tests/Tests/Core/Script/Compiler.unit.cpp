/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include <doctest/doctest.h>

#include <string>

#include "Core/Script/Compiler.hpp"
#include "Core/Config/Location.hpp"
#include "Core/Error/Handler.hpp"

TEST_SUITE("Script::Compiler") {
  using Variables = litr::Script::Compiler::Variables;
  using Location = litr::Config::Location;
  using Compiler = litr::Script::Compiler;

  // Success
  TEST_CASE("Single string") {
    const std::string source{"echo '%{'Hello'}'"};
    Location location{1, 1, R"(script = "echo '%{'Hello'}'")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::Error::Handler::HasErrors());
    CHECK_EQ(compiler.GetScript(), "echo 'Hello'");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Single string in the middle") {
    const std::string source{"echo '%{'Hello'}' and more"};
    Location location{1, 1, R"(script = "echo '%{'Hello'}' and more")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::Error::Handler::HasErrors());
    CHECK_EQ(compiler.GetScript(), "echo 'Hello' and more");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Single variable") {
    const std::string source{"echo '%{target}'"};
    Location location{1, 1, R"(script = "echo '%{target}'")"};
    Variables variables{{
        {"target", litr::CLI::Variable("target", std::string("Hello"))}
    }};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::Error::Handler::HasErrors());
    CHECK_EQ(compiler.GetScript(), "echo 'Hello'");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Single variable in the middle") {
    const std::string source{"echo '%{target}' and more"};
    Location location{1, 1, R"(script = "echo '%{target}' and more")"};
    Variables variables{{
        {"target", litr::CLI::Variable("target", std::string("Hello"))}
    }};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::Error::Handler::HasErrors());
    CHECK_EQ(compiler.GetScript(), "echo 'Hello' and more");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Single true boolean variable") {
    const std::string source{"echo %{target 'Hello'}"};
    Location location{1, 1, R"(script = "echo %{target 'Hello'}")"};
    Variables variables{{
        {"target", litr::CLI::Variable("target", true)}
    }};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::Error::Handler::HasErrors());
    CHECK_EQ(compiler.GetScript(), "echo Hello");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Single false boolean variable") {
    const std::string source{"echo %{target 'Hello'}"};
    Location location{1, 1, R"(script = "echo %{target 'Hello'}")"};
    Variables variables{
        {"target", litr::CLI::Variable("target", false)}
    };
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::Error::Handler::HasErrors());
    CHECK_EQ(compiler.GetScript(), "echo ");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Boolean variable printing value of second variable") {
    const std::string source{"echo '%{target value}'"};
    Location location{1, 1, R"(script = "echo '%{target value}'")"};
    Variables variables{{
        {"target", litr::CLI::Variable("target", true)},
        {"value", litr::CLI::Variable("value", std::string("Hello"))}
    }};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::Error::Handler::HasErrors());
    CHECK_EQ(compiler.GetScript(), "echo 'Hello'");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Boolean variable not printing value of second variable") {
    const std::string source{"echo '%{target value}'"};
    Location location{1, 1, R"(script = "echo '%{target value}'")"};
    Variables variables{{
        {"target", litr::CLI::Variable("target", false)},
        {"value", litr::CLI::Variable("value", std::string("Hello"))}
    }};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::Error::Handler::HasErrors());
    CHECK_EQ(compiler.GetScript(), "echo ''");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Variable with true or-statement") {
    const std::string source{"echo %{target 'Hello' or 'Bye'}"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{{
        {"target", litr::CLI::Variable("target", true)}
    }};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::Error::Handler::HasErrors());
    CHECK_EQ(compiler.GetScript(), "echo Hello");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Variable with false or-statement") {
    const std::string source{R"(echo %{target 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{{
        {"target", litr::CLI::Variable("target", false)}
    }};
    Compiler compiler{source, location, variables};

    CHECK_FALSE(litr::Error::Handler::HasErrors());
    CHECK_EQ(compiler.GetScript(), "echo Bye");
    litr::Error::Handler::Flush();
  }

  // Error

  TEST_CASE("Variable with undefined variable or-statement") {
    const std::string source{R"(echo %{target 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK(litr::Error::Handler::HasErrors());

    auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `target`: Undefined parameter.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Nested true statements will fail") {
    const std::string source{R"(echo %{a b 'Wrong' 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{
        {"a", litr::CLI::Variable("a", true)},
        {"b", litr::CLI::Variable("b", true)}
    };
    Compiler compiler{source, location, variables};

    CHECK(litr::Error::Handler::HasErrors());

    auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `'Hello'`: Expected `}`.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Nested false and true statements will fail") {
    const std::string source{R"(echo %{a b 'Wrong' 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{{
        {"a", litr::CLI::Variable("a", true)},
        {"b", litr::CLI::Variable("b", false)}
    }};
    Compiler compiler{source, location, variables};

    CHECK(litr::Error::Handler::HasErrors());

    auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `'Hello'`: Expected `}`.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Nested true and false statements will fail") {
    const std::string source{R"(echo %{a b 'Wrong' 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{{
        {"a", litr::CLI::Variable("a", false)},
        {"b", litr::CLI::Variable("b", true)}
    }};
    Compiler compiler{source, location, variables};

    CHECK(litr::Error::Handler::HasErrors());

    auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `'Wrong'`: Expected `}`.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Nested false statements will fail") {
    const std::string source{R"(echo %{a b 'Wrong' 'Hello' or 'Bye'})"};
    Location location{1, 1, R"(script = "echo %{target 'Hello' or 'Bye'}")"};
    Variables variables{{
        {"a", litr::CLI::Variable("a", false)},
        {"b", litr::CLI::Variable("b", false)}
    }};
    Compiler compiler{source, location, variables};

    CHECK(litr::Error::Handler::HasErrors());

    auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `'Wrong'`: Expected `}`.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Multiple strings in a row fail") {
    const std::string source{"echo '%{'Hello' ' ' 'World'}'"};
    Location location{1, 1, R"(script = "echo '%{'Hello' ' ' 'World'}'")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK(litr::Error::Handler::HasErrors());

    auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `' '`: Expected `}`.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Does throw on multiple non boolean variables in a row") {
    const std::string source{"echo '%{target value}'"};
    Location location{1, 1, R"(script = "echo '%{target value}'")"};
    Variables variables{{
        {"target", litr::CLI::Variable("target", std::string("Hello"))},
        {"value", litr::CLI::Variable("value", std::string(" World"))}
    }};
    Compiler compiler{source, location, variables};

    CHECK(litr::Error::Handler::HasErrors());

    auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `value`: Expected `}`.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Fails on duplicated 'or' for true case") {
    const std::string source{"echo '%{target 'a' or or 'b'}'"};
    Location location{1, 1, R"(script = "echo '%{target 'a' or or 'b'}'")"};
    Variables variables{{
        {"target", litr::CLI::Variable("target", true)}
    }};
    Compiler compiler{source, location, variables};

    CHECK(litr::Error::Handler::HasErrors());

    auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `'b'`: Expected `}`.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Fails on duplicated 'or' for false case") {
    const std::string source{"echo '%{target 'a' or or 'b'}'"};
    Location location{1, 1, R"(script = "echo '%{target 'a' or or 'b'}'")"};
    Variables variables{{
        {"target", litr::CLI::Variable("target", false)}
    }};
    Compiler compiler{source, location, variables};

    CHECK(litr::Error::Handler::HasErrors());

    auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `'b'`: Expected `}`.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Failed on single or") {
    const std::string source{"echo %{or}"};
    Location location{1, 1, R"(script = "echo %{or}")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK(litr::Error::Handler::HasErrors());

    auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `or`: Unexpected character.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Failed on single comma") {
    const std::string source{"echo %{,}"};
    Location location{1, 1, R"(script = "echo %{,}")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK(litr::Error::Handler::HasErrors());

    auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `,`: Unexpected character.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Failed on standalone parens") {
    const std::string source{"echo %{()}"};
    Location location{1, 1, R"(script = "echo %{()}")"};
    Variables variables{};
    Compiler compiler{source, location, variables};

    CHECK(litr::Error::Handler::HasErrors());

    auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `(`: Unexpected character.");
    litr::Error::Handler::Flush();
  }
}
