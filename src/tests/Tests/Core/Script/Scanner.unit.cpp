/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include <doctest/doctest.h>
#include <array>

// Not sure why, but doctest really wants iostream here when
// using g++. Who am I to judge.
#include <iostream>

#include "Core/Script/Scanner.hpp"

#define CHECK_DEFINITION(scanner, definition)                            \
  {                                                                      \
    for (auto&& test : (definition)) {                                   \
      litr::Script::Token token{(scanner).ScanToken()};                  \
      CHECK_EQ(token.Type, test.Type);                                   \
      CHECK_EQ(litr::Script::Scanner::GetTokenValue(token), test.Value); \
    }                                                                    \
  }

#define CHECK_EOS_TOKEN(scanner)                             \
  {                                                          \
    litr::Script::Token eos{(scanner).ScanToken()};          \
    CHECK_EQ(eos.Type, litr::Script::TokenType::EOS);        \
    CHECK_EQ(litr::Script::Scanner::GetTokenValue(eos), ""); \
  }

struct TokenDefinition {
  litr::Script::TokenType Type;
  std::string Value;
};

TEST_SUITE("Script::Scanner") {
  // Successful cases
  TEST_CASE("Nothing to parse") {
    litr::Script::Scanner scanner{"echo 'Hello World!'"};

    std::array<TokenDefinition, 1> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo 'Hello World!'"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Nothing to parse on escaped sequence") {
    litr::Script::Scanner scanner{R"(echo \%{something})"};

    std::array<TokenDefinition, 1> definition{{
        {litr::Script::TokenType::UNTOUCHED, R"(echo \%{something})"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single variable") {
    litr::Script::Scanner scanner{"echo %{var}"};

    std::array<TokenDefinition, 4> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::IDENTIFIER, "var"},
        {litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Ignores duplicated closing brace") {
    litr::Script::Scanner scanner{"echo %{var}}"};

    std::array<TokenDefinition, 5> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::IDENTIFIER, "var"},
        {litr::Script::TokenType::END_SEQ, "}"},
        {litr::Script::TokenType::UNTOUCHED, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple variables") {
    litr::Script::Scanner scanner{"echo %{var1} and %{var2}"};

    std::array<TokenDefinition, 8> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::IDENTIFIER, "var1"},
        {litr::Script::TokenType::END_SEQ, "}"},
        {litr::Script::TokenType::UNTOUCHED, " and "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::IDENTIFIER, "var2"},
        {litr::Script::TokenType::END_SEQ, "}"},
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple variables in between") {
    litr::Script::Scanner scanner{"echo %{var1} and %{var2} end"};

    std::array<TokenDefinition, 9> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::IDENTIFIER, "var1"},
        {litr::Script::TokenType::END_SEQ, "}"},
        {litr::Script::TokenType::UNTOUCHED, " and "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::IDENTIFIER, "var2"},
        {litr::Script::TokenType::END_SEQ, "}"},
        {litr::Script::TokenType::UNTOUCHED, " end"},
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single string") {
    litr::Script::Scanner scanner{"echo %{'Hello'}"};

    std::array<TokenDefinition, 4> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::STRING, "'Hello'"},
        {litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single string including ignored expression syntax") {
    litr::Script::Scanner scanner{"echo %{'He%{ll}o'}"};

    std::array<TokenDefinition, 4> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::STRING, "'He%{ll}o'"},
        {litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single string with leading variable") {
    litr::Script::Scanner scanner{"echo %{var 'Hello'}"};

    std::array<TokenDefinition, 5> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::IDENTIFIER, "var"},
        {litr::Script::TokenType::STRING, "'Hello'"},
        {litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Strings and or-statement") {
    litr::Script::Scanner scanner{"echo %{var 'Hello' or 'Bye'}"};

    std::array<TokenDefinition, 7> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::IDENTIFIER, "var"},
        {litr::Script::TokenType::STRING, "'Hello'"},
        {litr::Script::TokenType::OR, "or"},
        {litr::Script::TokenType::STRING, "'Bye'"},
        {litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  /*
  TEST_CASE("Strings and and-statement") {
    litr::Script::Scanner scanner{"echo %{var1 and var2 'Hello'}"};

    std::array<TokenDefinition, 7> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::IDENTIFIER, "var1"},
        {litr::Script::TokenType::AND, "and"},
        {litr::Script::TokenType::IDENTIFIER, "var2"},
        {litr::Script::TokenType::STRING, "'Hello'"},
        {litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }
  */

  TEST_CASE("Function usage with single argument") {
    litr::Script::Scanner scanner{"echo %{case_camel(var)}"};

    std::array<TokenDefinition, 7> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::IDENTIFIER, "case_camel"},
        {litr::Script::TokenType::LEFT_PAREN, "("},
        {litr::Script::TokenType::IDENTIFIER, "var"},
        {litr::Script::TokenType::RIGHT_PAREN, ")"},
        {litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Function usage with two arguments") {
    litr::Script::Scanner scanner{"echo %{case_camel(var1, var2)}"};

    std::array<TokenDefinition, 9> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::IDENTIFIER, "case_camel"},
        {litr::Script::TokenType::LEFT_PAREN, "("},
        {litr::Script::TokenType::IDENTIFIER, "var1"},
        {litr::Script::TokenType::COMMA, ","},
        {litr::Script::TokenType::IDENTIFIER, "var2"},
        {litr::Script::TokenType::RIGHT_PAREN, ")"},
        {litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Function usage with or-statement") {
    litr::Script::Scanner scanner{"echo %{fn(var) 'First' or 'Second'}"};

    std::array<TokenDefinition, 10> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::IDENTIFIER, "fn"},
        {litr::Script::TokenType::LEFT_PAREN, "("},
        {litr::Script::TokenType::IDENTIFIER, "var"},
        {litr::Script::TokenType::RIGHT_PAREN, ")"},
        {litr::Script::TokenType::STRING, "'First'"},
        {litr::Script::TokenType::OR, "or"},
        {litr::Script::TokenType::STRING, "'Second'"},
        {litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Nested function usage") {
    litr::Script::Scanner scanner{"echo %{fn1(fn2(var1), var2)}"};

    std::array<TokenDefinition, 12> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::IDENTIFIER, "fn1"},
        {litr::Script::TokenType::LEFT_PAREN, "("},
        {litr::Script::TokenType::IDENTIFIER, "fn2"},
        {litr::Script::TokenType::LEFT_PAREN, "("},
        {litr::Script::TokenType::IDENTIFIER, "var1"},
        {litr::Script::TokenType::RIGHT_PAREN, ")"},
        {litr::Script::TokenType::COMMA, ","},
        {litr::Script::TokenType::IDENTIFIER, "var2"},
        {litr::Script::TokenType::RIGHT_PAREN, ")"},
        {litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Does not care about invalid semantic") {
    litr::Script::Scanner scanner{"echo %{var"};

    std::array<TokenDefinition, 3> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::IDENTIFIER, "var"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Ignores percentage sign without opening brace") {
    litr::Script::Scanner scanner{"echo %var"};

    std::array<TokenDefinition, 1> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo %var"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  // Error cases
  TEST_CASE("Unterminated string") {
    litr::Script::Scanner scanner{"echo %{'Hello}"};

    std::array<TokenDefinition, 3> definition{{
        {litr::Script::TokenType::UNTOUCHED, "echo "},
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::ERROR, "Unterminated string."}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates unknown characters, but does not stop scanning") {
    litr::Script::Scanner scanner{"%{? fn}"};

    std::array<TokenDefinition, 4> definition{{
        {litr::Script::TokenType::START_SEQ, "%{"},
        {litr::Script::TokenType::ERROR, "Unexpected character."},
        {litr::Script::TokenType::IDENTIFIER, "fn"},
        {litr::Script::TokenType::END_SEQ, "}"},
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }
}
