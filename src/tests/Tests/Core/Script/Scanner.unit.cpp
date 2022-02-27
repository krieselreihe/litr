/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include <doctest/doctest.h>
#include <array>

// Not sure why, but doctest really wants iostream here when
// using g++. Who am I to judge.
#include <iostream>

#include "Core/Script/Scanner.hpp"

#define CHECK_DEFINITION(scanner, definition)                              \
  {                                                                        \
    for (auto&& test : (definition)) {                                     \
      litr::script::Token token{(scanner).scan_token()};                   \
      CHECK_EQ(token.type, test.type);                                     \
      CHECK_EQ(litr::script::Scanner::get_token_value(token), test.value); \
    }                                                                      \
  }

#define CHECK_EOS_TOKEN(scanner)                               \
  {                                                            \
    litr::script::Token eos{(scanner).scan_token()};           \
    CHECK_EQ(eos.type, litr::script::TokenType::EOS);          \
    CHECK_EQ(litr::script::Scanner::get_token_value(eos), ""); \
  }

struct TokenDefinition {
  litr::script::TokenType type;
  std::string value;
};

TEST_SUITE("script::Scanner") {
  // Successful cases
  TEST_CASE("Nothing to parse") {
    litr::script::Scanner scanner{"echo 'Hello World!'"};

    std::array<TokenDefinition, 1> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo 'Hello World!'"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Nothing to parse on escaped sequence") {
    litr::script::Scanner scanner{R"(echo \%{something})"};

    std::array<TokenDefinition, 1> definition{{
        {litr::script::TokenType::UNTOUCHED, R"(echo \%{something})"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single variable") {
    litr::script::Scanner scanner{"echo %{var}"};

    std::array<TokenDefinition, 4> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::IDENTIFIER, "var"},
        {litr::script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Ignores duplicated closing brace") {
    litr::script::Scanner scanner{"echo %{var}}"};

    std::array<TokenDefinition, 5> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::IDENTIFIER, "var"},
        {litr::script::TokenType::END_SEQ, "}"},
        {litr::script::TokenType::UNTOUCHED, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple variables") {
    litr::script::Scanner scanner{"echo %{var1} and %{var2}"};

    std::array<TokenDefinition, 8> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::IDENTIFIER, "var1"},
        {litr::script::TokenType::END_SEQ, "}"},
        {litr::script::TokenType::UNTOUCHED, " and "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::IDENTIFIER, "var2"},
        {litr::script::TokenType::END_SEQ, "}"},
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple variables in between") {
    litr::script::Scanner scanner{"echo %{var1} and %{var2} end"};

    std::array<TokenDefinition, 9> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::IDENTIFIER, "var1"},
        {litr::script::TokenType::END_SEQ, "}"},
        {litr::script::TokenType::UNTOUCHED, " and "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::IDENTIFIER, "var2"},
        {litr::script::TokenType::END_SEQ, "}"},
        {litr::script::TokenType::UNTOUCHED, " end"},
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single string") {
    litr::script::Scanner scanner{"echo %{'Hello'}"};

    std::array<TokenDefinition, 4> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::STRING, "'Hello'"},
        {litr::script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single string including ignored expression syntax") {
    litr::script::Scanner scanner{"echo %{'He%{ll}o'}"};

    std::array<TokenDefinition, 4> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::STRING, "'He%{ll}o'"},
        {litr::script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single string with leading variable") {
    litr::script::Scanner scanner{"echo %{var 'Hello'}"};

    std::array<TokenDefinition, 5> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::IDENTIFIER, "var"},
        {litr::script::TokenType::STRING, "'Hello'"},
        {litr::script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Strings and or-statement") {
    litr::script::Scanner scanner{"echo %{var 'Hello' or 'Bye'}"};

    std::array<TokenDefinition, 7> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::IDENTIFIER, "var"},
        {litr::script::TokenType::STRING, "'Hello'"},
        {litr::script::TokenType::OR, "or"},
        {litr::script::TokenType::STRING, "'Bye'"},
        {litr::script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  /*
  TEST_CASE("Strings and and-statement") {
    litr::script::Scanner scanner{"echo %{var1 and var2 'Hello'}"};

    std::array<TokenDefinition, 7> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::IDENTIFIER, "var1"},
        {litr::script::TokenType::AND, "and"},
        {litr::script::TokenType::IDENTIFIER, "var2"},
        {litr::script::TokenType::STRING, "'Hello'"},
        {litr::script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }
  */

  TEST_CASE("Function usage with single argument") {
    litr::script::Scanner scanner{"echo %{case_camel(var)}"};

    std::array<TokenDefinition, 7> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::IDENTIFIER, "case_camel"},
        {litr::script::TokenType::LEFT_PAREN, "("},
        {litr::script::TokenType::IDENTIFIER, "var"},
        {litr::script::TokenType::RIGHT_PAREN, ")"},
        {litr::script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Function usage with two arguments") {
    litr::script::Scanner scanner{"echo %{case_camel(var1, var2)}"};

    std::array<TokenDefinition, 9> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::IDENTIFIER, "case_camel"},
        {litr::script::TokenType::LEFT_PAREN, "("},
        {litr::script::TokenType::IDENTIFIER, "var1"},
        {litr::script::TokenType::COMMA, ","},
        {litr::script::TokenType::IDENTIFIER, "var2"},
        {litr::script::TokenType::RIGHT_PAREN, ")"},
        {litr::script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Function usage with or-statement") {
    litr::script::Scanner scanner{"echo %{fn(var) 'First' or 'Second'}"};

    std::array<TokenDefinition, 10> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::IDENTIFIER, "fn"},
        {litr::script::TokenType::LEFT_PAREN, "("},
        {litr::script::TokenType::IDENTIFIER, "var"},
        {litr::script::TokenType::RIGHT_PAREN, ")"},
        {litr::script::TokenType::STRING, "'First'"},
        {litr::script::TokenType::OR, "or"},
        {litr::script::TokenType::STRING, "'Second'"},
        {litr::script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Nested function usage") {
    litr::script::Scanner scanner{"echo %{fn1(fn2(var1), var2)}"};

    std::array<TokenDefinition, 12> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::IDENTIFIER, "fn1"},
        {litr::script::TokenType::LEFT_PAREN, "("},
        {litr::script::TokenType::IDENTIFIER, "fn2"},
        {litr::script::TokenType::LEFT_PAREN, "("},
        {litr::script::TokenType::IDENTIFIER, "var1"},
        {litr::script::TokenType::RIGHT_PAREN, ")"},
        {litr::script::TokenType::COMMA, ","},
        {litr::script::TokenType::IDENTIFIER, "var2"},
        {litr::script::TokenType::RIGHT_PAREN, ")"},
        {litr::script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Does not care about invalid semantic") {
    litr::script::Scanner scanner{"echo %{var"};

    std::array<TokenDefinition, 3> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::IDENTIFIER, "var"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Ignores percentage sign without opening brace") {
    litr::script::Scanner scanner{"echo %var"};

    std::array<TokenDefinition, 1> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo %var"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  // Error cases
  TEST_CASE("Unterminated string") {
    litr::script::Scanner scanner{"echo %{'Hello}"};

    std::array<TokenDefinition, 3> definition{{
        {litr::script::TokenType::UNTOUCHED, "echo "},
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::ERROR, "Unterminated string."}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates unknown characters, but does not stop scanning") {
    litr::script::Scanner scanner{"%{? fn}"};

    std::array<TokenDefinition, 4> definition{{
        {litr::script::TokenType::START_SEQ, "%{"},
        {litr::script::TokenType::ERROR, "Unexpected character."},
        {litr::script::TokenType::IDENTIFIER, "fn"},
        {litr::script::TokenType::END_SEQ, "}"},
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }
}
