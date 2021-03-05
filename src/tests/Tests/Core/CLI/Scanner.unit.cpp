#include <doctest/doctest.h>
#include <array>

// Not sure why, but doctest really wants iostream here when
// using g++. Who am I to judge.
#include <iostream>

#include "Core/CLI/Scanner.hpp"

#define CHECK_DEFINITION(scanner, definition)                        \
  {                                                                  \
    for (auto&& test : (definition)) {                               \
      Litr::CLI::Token token{(scanner).ScanToken()};                 \
      CHECK(token.Type == test.Type);                                \
      CHECK(Litr::CLI::Scanner::GetTokenValue(token) == test.Value); \
    }                                                                \
  }

#define CHECK_EOS_TOKEN(scanner)                         \
  {                                                      \
    Litr::CLI::Token eos{(scanner).ScanToken()};         \
    CHECK(eos.Type == Litr::CLI::TokenType::EOS);        \
    CHECK(Litr::CLI::Scanner::GetTokenValue(eos) == ""); \
  }

struct TokenDefinition {
  Litr::CLI::TokenType Type;
  std::string Value;
} __attribute__((aligned(32)));

TEST_SUITE("Scanner") {
  // Successful cases
  TEST_CASE("Single long parameter") {
    Litr::CLI::Scanner scanner{"--help"};

    std::array<TokenDefinition, 1> definition{{
        {Litr::CLI::TokenType::LONG_PARAMETER, "--help"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single short parameter") {
    Litr::CLI::Scanner scanner{"-h"};

    std::array<TokenDefinition, 1> definition{{
        {Litr::CLI::TokenType::SHORT_PARAMETER, "-h"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single command") {
    Litr::CLI::Scanner scanner{"build"};

    std::array<TokenDefinition, 1> definition{{
        {Litr::CLI::TokenType::COMMAND, "build"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple commands") {
    Litr::CLI::Scanner scanner{"build cpp"};

    std::array<TokenDefinition, 2> definition{{
        {Litr::CLI::TokenType::COMMAND, "build"},
        {Litr::CLI::TokenType::COMMAND, "cpp"},
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("More multiple commands with whitespace") {
    Litr::CLI::Scanner scanner{"build cpp   another"};

    std::array<TokenDefinition, 3> definition{{
        {Litr::CLI::TokenType::COMMAND, "build"},
        {Litr::CLI::TokenType::COMMAND, "cpp"},
        {Litr::CLI::TokenType::COMMAND, "another"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple comma separated commands") {
    Litr::CLI::Scanner scanner{"build,run"};

    std::array<TokenDefinition, 3> definition{{
        {Litr::CLI::TokenType::COMMAND, "build"},
        {Litr::CLI::TokenType::COMMA, ","},
        {Litr::CLI::TokenType::COMMAND, "run"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple comma separated commands with whitespace") {
    Litr::CLI::Scanner scanner{"build ,  run"};

    std::array<TokenDefinition, 3> definition{{
        {Litr::CLI::TokenType::COMMAND, "build"},
        {Litr::CLI::TokenType::COMMA, ","},
        {Litr::CLI::TokenType::COMMAND, "run"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Long parameter with string values") {
    Litr::CLI::Scanner scanner{" --target=\"release value\" build,run"};

    std::array<TokenDefinition, 6> definition{{
        {Litr::CLI::TokenType::LONG_PARAMETER, "--target"},
        {Litr::CLI::TokenType::EQUAL, "="},
        {Litr::CLI::TokenType::STRING, "\"release value\""},
        {Litr::CLI::TokenType::COMMAND, "build"},
        {Litr::CLI::TokenType::COMMA, ","},
        {Litr::CLI::TokenType::COMMAND, "run"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Long parameter with string values with whitespace") {
    Litr::CLI::Scanner scanner{"  --target  = \"release\"  build ,run  "};

    std::array<TokenDefinition, 6> definition{{
        {Litr::CLI::TokenType::LONG_PARAMETER, "--target"},
        {Litr::CLI::TokenType::EQUAL, "="},
        {Litr::CLI::TokenType::STRING, "\"release\""},
        {Litr::CLI::TokenType::COMMAND, "build"},
        {Litr::CLI::TokenType::COMMA, ","},
        {Litr::CLI::TokenType::COMMAND, "run"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Short parameter with number values") {
    Litr::CLI::Scanner scanner{" -t=42"};

    std::array<TokenDefinition, 3> definition{{
        {Litr::CLI::TokenType::SHORT_PARAMETER, "-t"},
        {Litr::CLI::TokenType::EQUAL, "="},
        {Litr::CLI::TokenType::NUMBER, "42"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Mixed parameters with mixed values and whitespace") {
    Litr::CLI::Scanner scanner{" -t=\"release\" --debug  -p = 2.45"};

    std::array<TokenDefinition, 7> definition{{
        {Litr::CLI::TokenType::SHORT_PARAMETER, "-t"},
        {Litr::CLI::TokenType::EQUAL, "="},
        {Litr::CLI::TokenType::STRING, "\"release\""},
        {Litr::CLI::TokenType::LONG_PARAMETER, "--debug"},
        {Litr::CLI::TokenType::SHORT_PARAMETER, "-p"},
        {Litr::CLI::TokenType::EQUAL, "="},
        {Litr::CLI::TokenType::NUMBER, "2.45"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Mixed source string") {
    Litr::CLI::Scanner scanner{"-t=\"release\" build, run --peer = 1.23"};

    std::array<TokenDefinition, 9> definition{{
        {Litr::CLI::TokenType::SHORT_PARAMETER, "-t"},
        {Litr::CLI::TokenType::EQUAL, "="},
        {Litr::CLI::TokenType::STRING, "\"release\""},
        {Litr::CLI::TokenType::COMMAND, "build"},
        {Litr::CLI::TokenType::COMMA, ","},
        {Litr::CLI::TokenType::COMMAND, "run"},
        {Litr::CLI::TokenType::LONG_PARAMETER, "--peer"},
        {Litr::CLI::TokenType::EQUAL, "="},
        {Litr::CLI::TokenType::NUMBER, "1.23"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Does not care about invalid semantics") {
    Litr::CLI::Scanner scanner{", , ,"};

    std::array<TokenDefinition, 3> definition{{
        {Litr::CLI::TokenType::COMMA, ","},
        {Litr::CLI::TokenType::COMMA, ","},
        {Litr::CLI::TokenType::COMMA, ","},
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  // Error cases
  TEST_CASE("Invalidates unterminated string syntax") {
    Litr::CLI::Scanner scanner{"\"str"};

    std::array<TokenDefinition, 1> definition{{
        {Litr::CLI::TokenType::ERROR, "Unterminated string."}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates unknown characters, but does not stop scanning") {
    Litr::CLI::Scanner scanner{"? -p"};

    std::array<TokenDefinition, 2> definition{{
        {Litr::CLI::TokenType::ERROR, "Unexpected character."},
        {Litr::CLI::TokenType::SHORT_PARAMETER, "-p"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates wrong long parameter start character") {
    Litr::CLI::Scanner scanner{"--1 --_bob"};

    std::array<TokenDefinition, 2> definition{{
        {Litr::CLI::TokenType::ERROR, "A parameter cannot start with a number or _ (allowed characters are A-Za-z)."},
        {Litr::CLI::TokenType::ERROR, "A parameter cannot start with a number or _ (allowed characters are A-Za-z)."}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates wrong short parameter name") {
    Litr::CLI::Scanner scanner{"-1 -_ -?"};

    std::array<TokenDefinition, 3> definition{{
        {Litr::CLI::TokenType::ERROR, "A short parameter can only be A-Za-z as name."},
        {Litr::CLI::TokenType::ERROR, "A short parameter can only be A-Za-z as name."},
        {Litr::CLI::TokenType::ERROR, "A short parameter can only be A-Za-z as name."}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates too long short parameter name") {
    Litr::CLI::Scanner scanner{"-long"};

    std::array<TokenDefinition, 1> definition{{
        {Litr::CLI::TokenType::ERROR, "A short parameter can only contain one character (A-Za-z)."}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }
}
