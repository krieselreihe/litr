/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include <doctest/doctest.h>
#include <array>

// Not sure why, but doctest really wants iostream here when
// using g++. Who am I to judge.
#include <iostream>

#include "Core/CLI/Scanner.hpp"

#define CHECK_DEFINITION(scanner, definition)                         \
  {                                                                   \
    for (auto&& test : (definition)) {                                \
      litr::CLI::Token token{(scanner).ScanToken()};                  \
      CHECK_EQ(token.Type, test.Type);                                \
      CHECK_EQ(litr::CLI::Scanner::GetTokenValue(token), test.Value); \
    }                                                                 \
  }

#define CHECK_EOS_TOKEN(scanner)                          \
  {                                                       \
    litr::CLI::Token eos{(scanner).ScanToken()};          \
    CHECK_EQ(eos.Type, litr::CLI::TokenType::EOS);        \
    CHECK_EQ(litr::CLI::Scanner::GetTokenValue(eos), ""); \
  }

struct TokenDefinition {
  litr::CLI::TokenType Type;
  std::string Value;
};

TEST_SUITE("CLI::Scanner") {
  // Successful cases
  TEST_CASE("Single long parameter") {
    litr::CLI::Scanner scanner{"--help"};

    std::array<TokenDefinition, 1> definition{{
        {litr::CLI::TokenType::LONG_PARAMETER, "--help"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single short parameter") {
    litr::CLI::Scanner scanner{"-h"};

    std::array<TokenDefinition, 1> definition{{
        {litr::CLI::TokenType::SHORT_PARAMETER, "-h"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single command") {
    litr::CLI::Scanner scanner{"build"};

    std::array<TokenDefinition, 1> definition{{
        {litr::CLI::TokenType::COMMAND, "build"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple commands") {
    litr::CLI::Scanner scanner{"build cpp"};

    std::array<TokenDefinition, 2> definition{{
        {litr::CLI::TokenType::COMMAND, "build"},
        {litr::CLI::TokenType::COMMAND, "cpp"},
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("More multiple commands with whitespace") {
    litr::CLI::Scanner scanner{"build cpp   another"};

    std::array<TokenDefinition, 3> definition{{
        {litr::CLI::TokenType::COMMAND, "build"},
        {litr::CLI::TokenType::COMMAND, "cpp"},
        {litr::CLI::TokenType::COMMAND, "another"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple comma separated commands") {
    litr::CLI::Scanner scanner{"build,run"};

    std::array<TokenDefinition, 3> definition{{
        {litr::CLI::TokenType::COMMAND, "build"},
        {litr::CLI::TokenType::COMMA, ","},
        {litr::CLI::TokenType::COMMAND, "run"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple comma separated commands with whitespace") {
    litr::CLI::Scanner scanner{"build ,  run"};

    std::array<TokenDefinition, 3> definition{{
        {litr::CLI::TokenType::COMMAND, "build"},
        {litr::CLI::TokenType::COMMA, ","},
        {litr::CLI::TokenType::COMMAND, "run"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Long parameter with string values") {
    litr::CLI::Scanner scanner{" --target=\"release value\" build,run"};

    std::array<TokenDefinition, 6> definition{{
        {litr::CLI::TokenType::LONG_PARAMETER, "--target"},
        {litr::CLI::TokenType::EQUAL, "="},
        {litr::CLI::TokenType::STRING, "\"release value\""},
        {litr::CLI::TokenType::COMMAND, "build"},
        {litr::CLI::TokenType::COMMA, ","},
        {litr::CLI::TokenType::COMMAND, "run"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Long parameter with string values with whitespace") {
    litr::CLI::Scanner scanner{"  --target  = \"release\"  build ,run  "};

    std::array<TokenDefinition, 6> definition{{
        {litr::CLI::TokenType::LONG_PARAMETER, "--target"},
        {litr::CLI::TokenType::EQUAL, "="},
        {litr::CLI::TokenType::STRING, "\"release\""},
        {litr::CLI::TokenType::COMMAND, "build"},
        {litr::CLI::TokenType::COMMA, ","},
        {litr::CLI::TokenType::COMMAND, "run"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Short parameter with number values") {
    litr::CLI::Scanner scanner{" -t=42"};

    std::array<TokenDefinition, 3> definition{{
        {litr::CLI::TokenType::SHORT_PARAMETER, "-t"},
        {litr::CLI::TokenType::EQUAL, "="},
        {litr::CLI::TokenType::NUMBER, "42"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Mixed parameters with mixed values and whitespace") {
    litr::CLI::Scanner scanner{" -t=\"release\" --debug  -p = 2.45"};

    std::array<TokenDefinition, 7> definition{{
        {litr::CLI::TokenType::SHORT_PARAMETER, "-t"},
        {litr::CLI::TokenType::EQUAL, "="},
        {litr::CLI::TokenType::STRING, "\"release\""},
        {litr::CLI::TokenType::LONG_PARAMETER, "--debug"},
        {litr::CLI::TokenType::SHORT_PARAMETER, "-p"},
        {litr::CLI::TokenType::EQUAL, "="},
        {litr::CLI::TokenType::NUMBER, "2.45"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Mixed source string") {
    litr::CLI::Scanner scanner{"-t=\"release\" build, run --peer = 1.23"};

    std::array<TokenDefinition, 9> definition{{
        {litr::CLI::TokenType::SHORT_PARAMETER, "-t"},
        {litr::CLI::TokenType::EQUAL, "="},
        {litr::CLI::TokenType::STRING, "\"release\""},
        {litr::CLI::TokenType::COMMAND, "build"},
        {litr::CLI::TokenType::COMMA, ","},
        {litr::CLI::TokenType::COMMAND, "run"},
        {litr::CLI::TokenType::LONG_PARAMETER, "--peer"},
        {litr::CLI::TokenType::EQUAL, "="},
        {litr::CLI::TokenType::NUMBER, "1.23"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Does not care about invalid semantics") {
    litr::CLI::Scanner scanner{", , ,"};

    std::array<TokenDefinition, 3> definition{{
        {litr::CLI::TokenType::COMMA, ","},
        {litr::CLI::TokenType::COMMA, ","},
        {litr::CLI::TokenType::COMMA, ","},
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  // Error cases
  TEST_CASE("Invalidates unterminated string syntax") {
    litr::CLI::Scanner scanner{"\"str"};

    std::array<TokenDefinition, 1> definition{{
        {litr::CLI::TokenType::ERROR, "Unterminated string."}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates unknown characters, but does not stop scanning") {
    litr::CLI::Scanner scanner{"? -p"};

    std::array<TokenDefinition, 2> definition{{
        {litr::CLI::TokenType::ERROR, "Unexpected character."},
        {litr::CLI::TokenType::SHORT_PARAMETER, "-p"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates wrong long parameter start character") {
    litr::CLI::Scanner scanner{"--1 --_bob"};

    std::array<TokenDefinition, 2> definition{{
        {litr::CLI::TokenType::ERROR, "A parameter can only start with the characters A-Za-z."},
        {litr::CLI::TokenType::ERROR, "A parameter can only start with the characters A-Za-z."}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates wrong short parameter name") {
    litr::CLI::Scanner scanner{"-1 -_ -?"};

    std::array<TokenDefinition, 3> definition{{
        {litr::CLI::TokenType::ERROR, "A short parameter can only be A-Za-z as name."},
        {litr::CLI::TokenType::ERROR, "A short parameter can only be A-Za-z as name."},
        {litr::CLI::TokenType::ERROR, "A short parameter can only be A-Za-z as name."}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates too long short parameter name") {
    litr::CLI::Scanner scanner{"-long"};

    std::array<TokenDefinition, 1> definition{{
        {litr::CLI::TokenType::ERROR, "A short parameter can only contain one character (A-Za-z)."}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates duplicated parameter initializer") {
    litr::CLI::Scanner scanner{"---long"};

    std::array<TokenDefinition, 1> definition{{
        {litr::CLI::TokenType::ERROR, "A parameter can only start with the characters A-Za-z."}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }
}
