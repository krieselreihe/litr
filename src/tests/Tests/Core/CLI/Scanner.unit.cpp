#include <doctest/doctest.h>
#include <array>

#include "Core/CLI/Scanner.hpp"

#define TEST_DEFINITION(scanner, definition)                    \
  {                                                             \
    for (auto&& test : (definition)) {                          \
      Litr::Scanner::Token token{(scanner).ScanToken()};        \
      CHECK(token.Line == test.Line);                           \
      CHECK(token.Type == test.Type);                           \
      CHECK(Litr::Scanner::GetTokenValue(token) == test.Value); \
    }                                                           \
  }

#define TEST_EOS_TOKEN(scanner)                       \
  {                                                   \
    Litr::Scanner::Token eos{(scanner).ScanToken()};  \
    CHECK(eos.Line == 1);                             \
    CHECK(eos.Type == Litr::Scanner::TokenType::EOS); \
    CHECK(Litr::Scanner::GetTokenValue(eos) == "");   \
  }

struct TokenDefinition {
  int Line;
  Litr::Scanner::TokenType Type;
  std::string Value;
} __attribute__((aligned(32)));

TEST_SUITE("Scanner") {
  // Successful cases
  TEST_CASE("Single long parameter") {
    Litr::Scanner scanner{"--help"};

    std::array<TokenDefinition, 1> definition{{
        {1, Litr::Scanner::TokenType::LONG_PARAMETER, "--help"}
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single short parameter") {
    Litr::Scanner scanner{"-h"};

    std::array<TokenDefinition, 1> definition{{
        {1, Litr::Scanner::TokenType::SHORT_PARAMETER, "-h"}
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single command") {
    Litr::Scanner scanner{"build"};

    std::array<TokenDefinition, 1> definition{{
        {1, Litr::Scanner::TokenType::COMMAND, "build"}
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple commands") {
    Litr::Scanner scanner{"build cpp"};

    std::array<TokenDefinition, 2> definition{{
        {1, Litr::Scanner::TokenType::COMMAND, "build"},
        {1, Litr::Scanner::TokenType::COMMAND, "cpp"},
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  TEST_CASE("More multiple commands with whitespace") {
    Litr::Scanner scanner{"build cpp   another"};

    std::array<TokenDefinition, 3> definition{{
        {1, Litr::Scanner::TokenType::COMMAND, "build"},
        {1, Litr::Scanner::TokenType::COMMAND, "cpp"},
        {1, Litr::Scanner::TokenType::COMMAND, "another"}
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple comma separated commands") {
    Litr::Scanner scanner{"build,run"};

    std::array<TokenDefinition, 3> definition{{
        {1, Litr::Scanner::TokenType::COMMAND, "build"},
        {1, Litr::Scanner::TokenType::COMMA, ","},
        {1, Litr::Scanner::TokenType::COMMAND, "run"}
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple comma separated commands with whitespace") {
    Litr::Scanner scanner{"build ,  run"};

    std::array<TokenDefinition, 3> definition{{
        {1, Litr::Scanner::TokenType::COMMAND, "build"},
        {1, Litr::Scanner::TokenType::COMMA, ","},
        {1, Litr::Scanner::TokenType::COMMAND, "run"}
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  TEST_CASE("Long parameter with string values") {
    Litr::Scanner scanner{" --target=\"release\" build,run"};

    std::array<TokenDefinition, 6> definition{{
        {1, Litr::Scanner::TokenType::LONG_PARAMETER, "--target"},
        {1, Litr::Scanner::TokenType::EQUAL, "="},
        {1, Litr::Scanner::TokenType::STRING, "\"release\""},
        {1, Litr::Scanner::TokenType::COMMAND, "build"},
        {1, Litr::Scanner::TokenType::COMMA, ","},
        {1, Litr::Scanner::TokenType::COMMAND, "run"}
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  TEST_CASE("Long parameter with string values with whitespace") {
    Litr::Scanner scanner{"  --target  = \"release\"  build ,run  "};

    std::array<TokenDefinition, 6> definition{{
        {1, Litr::Scanner::TokenType::LONG_PARAMETER, "--target"},
        {1, Litr::Scanner::TokenType::EQUAL, "="},
        {1, Litr::Scanner::TokenType::STRING, "\"release\""},
        {1, Litr::Scanner::TokenType::COMMAND, "build"},
        {1, Litr::Scanner::TokenType::COMMA, ","},
        {1, Litr::Scanner::TokenType::COMMAND, "run"}
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  TEST_CASE("Short parameter with number values") {
    Litr::Scanner scanner{" -t=42"};

    std::array<TokenDefinition, 3> definition{{
        {1, Litr::Scanner::TokenType::SHORT_PARAMETER, "-t"},
        {1, Litr::Scanner::TokenType::EQUAL, "="},
        {1, Litr::Scanner::TokenType::NUMBER, "42"}
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  TEST_CASE("Mixed parameters with mixed values and whitespace") {
    Litr::Scanner scanner{" -t=\"release\" --debug  -p = 2.45"};

    std::array<TokenDefinition, 7> definition{{
        {1, Litr::Scanner::TokenType::SHORT_PARAMETER, "-t"},
        {1, Litr::Scanner::TokenType::EQUAL, "="},
        {1, Litr::Scanner::TokenType::STRING, "\"release\""},
        {1, Litr::Scanner::TokenType::LONG_PARAMETER, "--debug"},
        {1, Litr::Scanner::TokenType::SHORT_PARAMETER, "-p"},
        {1, Litr::Scanner::TokenType::EQUAL, "="},
        {1, Litr::Scanner::TokenType::NUMBER, "2.45"}
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  TEST_CASE("Mixed source string") {
    Litr::Scanner scanner{"-t=\"release\" build, run --peer = 1.23"};

    std::array<TokenDefinition, 9> definition{{
        {1, Litr::Scanner::TokenType::SHORT_PARAMETER, "-t"},
        {1, Litr::Scanner::TokenType::EQUAL, "="},
        {1, Litr::Scanner::TokenType::STRING, "\"release\""},
        {1, Litr::Scanner::TokenType::COMMAND, "build"},
        {1, Litr::Scanner::TokenType::COMMA, ","},
        {1, Litr::Scanner::TokenType::COMMAND, "run"},
        {1, Litr::Scanner::TokenType::LONG_PARAMETER, "--peer"},
        {1, Litr::Scanner::TokenType::EQUAL, "="},
        {1, Litr::Scanner::TokenType::NUMBER, "1.23"}
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  TEST_CASE("Does not care about invalid semantics") {
    Litr::Scanner scanner{", , ,"};

    std::array<TokenDefinition, 3> definition{{
        {1, Litr::Scanner::TokenType::COMMA, ","},
        {1, Litr::Scanner::TokenType::COMMA, ","},
        {1, Litr::Scanner::TokenType::COMMA, ","},
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  // Error cases
  TEST_CASE("Invalidates unterminated string syntax") {
    Litr::Scanner scanner{"\"str"};

    std::array<TokenDefinition, 1> definition{{
        {1, Litr::Scanner::TokenType::ERROR, "Unterminated string."}
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates unknown characters, but does not stop scanning") {
    Litr::Scanner scanner{"? -p"};

    std::array<TokenDefinition, 2> definition{{
        {1, Litr::Scanner::TokenType::ERROR, "Unexpected character."},
        {1, Litr::Scanner::TokenType::SHORT_PARAMETER, "-p"}
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates wrong long parameter start character") {
    Litr::Scanner scanner{"--1 --_bob"};

    std::array<TokenDefinition, 2> definition{{
        {1, Litr::Scanner::TokenType::ERROR, "A parameter cannot start with a number or _ (allowed characters are A-Za-z)."},
        {1, Litr::Scanner::TokenType::ERROR, "A parameter cannot start with a number or _ (allowed characters are A-Za-z)."}
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates wrong short parameter name") {
    Litr::Scanner scanner{"-1 -_ -?"};

    std::array<TokenDefinition, 3> definition{{
        {1, Litr::Scanner::TokenType::ERROR, "A short parameter can only be A-Za-z as name."},
        {1, Litr::Scanner::TokenType::ERROR, "A short parameter can only be A-Za-z as name."},
        {1, Litr::Scanner::TokenType::ERROR, "A short parameter can only be A-Za-z as name."}
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates too long short parameter name") {
    Litr::Scanner scanner{"-long"};

    std::array<TokenDefinition, 1> definition{{
        {1, Litr::Scanner::TokenType::ERROR, "A short parameter can only contain one character (A-Za-z)."}
    }};

    TEST_DEFINITION(scanner, definition);
    TEST_EOS_TOKEN(scanner);
  }
}
