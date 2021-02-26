#include <doctest/doctest.h>
#include <array>

#include "Core/CLI/Token.hpp"
#include "Core/CLI/Scanner.hpp"

#define CHECK_DEFINITION(scanner, definition)                   \
  {                                                             \
    for (auto&& test : (definition)) {                          \
      Litr::Token token{(scanner).ScanToken()};                 \
      CHECK(token.Type == test.Type);                           \
      CHECK(Litr::Scanner::GetTokenValue(token) == test.Value); \
    }                                                           \
  }

#define CHECK_EOS_TOKEN(scanner)                    \
  {                                                 \
    Litr::Token eos{(scanner).ScanToken()};         \
    CHECK(eos.Type == Litr::TokenType::EOS);        \
    CHECK(Litr::Scanner::GetTokenValue(eos) == ""); \
  }

struct TokenDefinition {
  Litr::TokenType Type;
  std::string Value;
} __attribute__((aligned(32)));

TEST_SUITE("Scanner") {
  // Successful cases
  TEST_CASE("Single long parameter") {
    Litr::Scanner scanner{"--help"};

    std::array<TokenDefinition, 1> definition{{
        {Litr::TokenType::LONG_PARAMETER, "--help"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single short parameter") {
    Litr::Scanner scanner{"-h"};

    std::array<TokenDefinition, 1> definition{{
        {Litr::TokenType::SHORT_PARAMETER, "-h"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single command") {
    Litr::Scanner scanner{"build"};

    std::array<TokenDefinition, 1> definition{{
        {Litr::TokenType::COMMAND, "build"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple commands") {
    Litr::Scanner scanner{"build cpp"};

    std::array<TokenDefinition, 2> definition{{
        {Litr::TokenType::COMMAND, "build"},
        {Litr::TokenType::COMMAND, "cpp"},
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("More multiple commands with whitespace") {
    Litr::Scanner scanner{"build cpp   another"};

    std::array<TokenDefinition, 3> definition{{
        {Litr::TokenType::COMMAND, "build"},
        {Litr::TokenType::COMMAND, "cpp"},
        {Litr::TokenType::COMMAND, "another"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple comma separated commands") {
    Litr::Scanner scanner{"build,run"};

    std::array<TokenDefinition, 3> definition{{
        {Litr::TokenType::COMMAND, "build"},
        {Litr::TokenType::COMMA, ","},
        {Litr::TokenType::COMMAND, "run"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple comma separated commands with whitespace") {
    Litr::Scanner scanner{"build ,  run"};

    std::array<TokenDefinition, 3> definition{{
        {Litr::TokenType::COMMAND, "build"},
        {Litr::TokenType::COMMA, ","},
        {Litr::TokenType::COMMAND, "run"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Long parameter with string values") {
    Litr::Scanner scanner{" --target=\"release value\" build,run"};

    std::array<TokenDefinition, 6> definition{{
        {Litr::TokenType::LONG_PARAMETER, "--target"},
        {Litr::TokenType::EQUAL, "="},
        {Litr::TokenType::STRING, "\"release value\""},
        {Litr::TokenType::COMMAND, "build"},
        {Litr::TokenType::COMMA, ","},
        {Litr::TokenType::COMMAND, "run"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Long parameter with string values with whitespace") {
    Litr::Scanner scanner{"  --target  = \"release\"  build ,run  "};

    std::array<TokenDefinition, 6> definition{{
        {Litr::TokenType::LONG_PARAMETER, "--target"},
        {Litr::TokenType::EQUAL, "="},
        {Litr::TokenType::STRING, "\"release\""},
        {Litr::TokenType::COMMAND, "build"},
        {Litr::TokenType::COMMA, ","},
        {Litr::TokenType::COMMAND, "run"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Short parameter with number values") {
    Litr::Scanner scanner{" -t=42"};

    std::array<TokenDefinition, 3> definition{{
        {Litr::TokenType::SHORT_PARAMETER, "-t"},
        {Litr::TokenType::EQUAL, "="},
        {Litr::TokenType::NUMBER, "42"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Mixed parameters with mixed values and whitespace") {
    Litr::Scanner scanner{" -t=\"release\" --debug  -p = 2.45"};

    std::array<TokenDefinition, 7> definition{{
        {Litr::TokenType::SHORT_PARAMETER, "-t"},
        {Litr::TokenType::EQUAL, "="},
        {Litr::TokenType::STRING, "\"release\""},
        {Litr::TokenType::LONG_PARAMETER, "--debug"},
        {Litr::TokenType::SHORT_PARAMETER, "-p"},
        {Litr::TokenType::EQUAL, "="},
        {Litr::TokenType::NUMBER, "2.45"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Mixed source string") {
    Litr::Scanner scanner{"-t=\"release\" build, run --peer = 1.23"};

    std::array<TokenDefinition, 9> definition{{
        {Litr::TokenType::SHORT_PARAMETER, "-t"},
        {Litr::TokenType::EQUAL, "="},
        {Litr::TokenType::STRING, "\"release\""},
        {Litr::TokenType::COMMAND, "build"},
        {Litr::TokenType::COMMA, ","},
        {Litr::TokenType::COMMAND, "run"},
        {Litr::TokenType::LONG_PARAMETER, "--peer"},
        {Litr::TokenType::EQUAL, "="},
        {Litr::TokenType::NUMBER, "1.23"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Does not care about invalid semantics") {
    Litr::Scanner scanner{", , ,"};

    std::array<TokenDefinition, 3> definition{{
        {Litr::TokenType::COMMA, ","},
        {Litr::TokenType::COMMA, ","},
        {Litr::TokenType::COMMA, ","},
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  // Error cases
  TEST_CASE("Invalidates unterminated string syntax") {
    Litr::Scanner scanner{"\"str"};

    std::array<TokenDefinition, 1> definition{{
        {Litr::TokenType::ERROR, "Unterminated string."}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates unknown characters, but does not stop scanning") {
    Litr::Scanner scanner{"? -p"};

    std::array<TokenDefinition, 2> definition{{
        {Litr::TokenType::ERROR, "Unexpected character."},
        {Litr::TokenType::SHORT_PARAMETER, "-p"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates wrong long parameter start character") {
    Litr::Scanner scanner{"--1 --_bob"};

    std::array<TokenDefinition, 2> definition{{
        {Litr::TokenType::ERROR, "A parameter cannot start with a number or _ (allowed characters are A-Za-z)."},
        {Litr::TokenType::ERROR, "A parameter cannot start with a number or _ (allowed characters are A-Za-z)."}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates wrong short parameter name") {
    Litr::Scanner scanner{"-1 -_ -?"};

    std::array<TokenDefinition, 3> definition{{
        {Litr::TokenType::ERROR, "A short parameter can only be A-Za-z as name."},
        {Litr::TokenType::ERROR, "A short parameter can only be A-Za-z as name."},
        {Litr::TokenType::ERROR, "A short parameter can only be A-Za-z as name."}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates too long short parameter name") {
    Litr::Scanner scanner{"-long"};

    std::array<TokenDefinition, 1> definition{{
        {Litr::TokenType::ERROR, "A short parameter can only contain one character (A-Za-z)."}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }
}
