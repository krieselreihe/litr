/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include <doctest/doctest.h>

#include <array>

// Not sure why, but doctest really wants iostream here when
// using g++. Who am I to judge.
#include <iostream>

#include "Core/Script/Scanner.hpp"

struct TokenDefinition {
  Litr::Script::TokenType type;
  std::string value;
};

template <typename T, size_t S>
inline void check_definition(Litr::Script::Scanner& scanner, const std::array<T, S>& definition) {
  for (auto&& test : definition) {
    Litr::Script::Token token{scanner.scan_token()};
    // NOLINTNEXTLINE
    CHECK_EQ(token.type, test.type);
    // NOLINTNEXTLINE
    CHECK_EQ(Litr::Script::Scanner::get_token_value(token), test.value);
  }
}

inline void check_eos_token(Litr::Script::Scanner& scanner) {
  Litr::Script::Token eos{(scanner).scan_token()};
  // NOLINTNEXTLINE
  CHECK_EQ(eos.type, Litr::Script::TokenType::EOS);
  // NOLINTNEXTLINE
  CHECK_EQ(Litr::Script::Scanner::get_token_value(eos), "");
}

TEST_SUITE("script::Scanner") {
  // Successful cases
  TEST_CASE("Nothing to parse") {
    Litr::Script::Scanner scanner{"echo 'Hello World!'"};

    std::array<TokenDefinition, 1> definition{
        {{Litr::Script::TokenType::UNTOUCHED, "echo 'Hello World!'"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Nothing to parse on escaped sequence") {
    Litr::Script::Scanner scanner{R"(echo \%{something})"};

    std::array<TokenDefinition, 1> definition{
        {{Litr::Script::TokenType::UNTOUCHED, R"(echo \%{something})"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Single variable") {
    Litr::Script::Scanner scanner{"echo %{var}"};

    std::array<TokenDefinition, 4> definition{{{Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "var"},
        {Litr::Script::TokenType::END_SEQ, "}"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Ignores duplicated closing brace") {
    Litr::Script::Scanner scanner{"echo %{var}}"};

    constexpr int item_count{5};
    std::array<TokenDefinition, item_count> definition{
        {{Litr::Script::TokenType::UNTOUCHED, "echo "},
            {Litr::Script::TokenType::START_SEQ, "%{"},
            {Litr::Script::TokenType::IDENTIFIER, "var"},
            {Litr::Script::TokenType::END_SEQ, "}"},
            {Litr::Script::TokenType::UNTOUCHED, "}"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Multiple variables") {
    Litr::Script::Scanner scanner{"echo %{var1} and %{var2}"};

    constexpr int item_count{8};
    std::array<TokenDefinition, item_count> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "var1"},
        {Litr::Script::TokenType::END_SEQ, "}"},
        {Litr::Script::TokenType::UNTOUCHED, " and "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "var2"},
        {Litr::Script::TokenType::END_SEQ, "}"},
    }};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Multiple variables in between") {
    Litr::Script::Scanner scanner{"echo %{var1} and %{var2} end"};

    constexpr int item_count{9};
    std::array<TokenDefinition, item_count> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "var1"},
        {Litr::Script::TokenType::END_SEQ, "}"},
        {Litr::Script::TokenType::UNTOUCHED, " and "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "var2"},
        {Litr::Script::TokenType::END_SEQ, "}"},
        {Litr::Script::TokenType::UNTOUCHED, " end"},
    }};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Single string") {
    Litr::Script::Scanner scanner{"echo %{'Hello'}"};

    std::array<TokenDefinition, 4> definition{{{Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::STRING, "'Hello'"},
        {Litr::Script::TokenType::END_SEQ, "}"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Single string including ignored expression syntax") {
    Litr::Script::Scanner scanner{"echo %{'He%{ll}o'}"};

    std::array<TokenDefinition, 4> definition{{{Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::STRING, "'He%{ll}o'"},
        {Litr::Script::TokenType::END_SEQ, "}"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Single string with leading variable") {
    Litr::Script::Scanner scanner{"echo %{var 'Hello'}"};

    constexpr int item_count{5};
    std::array<TokenDefinition, item_count> definition{
        {{Litr::Script::TokenType::UNTOUCHED, "echo "},
            {Litr::Script::TokenType::START_SEQ, "%{"},
            {Litr::Script::TokenType::IDENTIFIER, "var"},
            {Litr::Script::TokenType::STRING, "'Hello'"},
            {Litr::Script::TokenType::END_SEQ, "}"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Strings and or-statement") {
    Litr::Script::Scanner scanner{"echo %{var 'Hello' or 'Bye'}"};

    constexpr int item_count{7};
    std::array<TokenDefinition, item_count> definition{
        {{Litr::Script::TokenType::UNTOUCHED, "echo "},
            {Litr::Script::TokenType::START_SEQ, "%{"},
            {Litr::Script::TokenType::IDENTIFIER, "var"},
            {Litr::Script::TokenType::STRING, "'Hello'"},
            {Litr::Script::TokenType::OR, "or"},
            {Litr::Script::TokenType::STRING, "'Bye'"},
            {Litr::Script::TokenType::END_SEQ, "}"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  /*
  TEST_CASE("Strings and and-statement") {
    Litr::Script::Scanner scanner{"echo %{var1 and var2 'Hello'}"};

    constexpr int item_count{7};
    std::array<TokenDefinition, item_count> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "var1"},
        {Litr::Script::TokenType::AND, "and"},
        {Litr::Script::TokenType::IDENTIFIER, "var2"},
        {Litr::Script::TokenType::STRING, "'Hello'"},
        {Litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }
  */

  TEST_CASE("Function usage with single argument") {
    Litr::Script::Scanner scanner{"echo %{case_camel(var)}"};

    constexpr int item_count{7};
    std::array<TokenDefinition, item_count> definition{
        {{Litr::Script::TokenType::UNTOUCHED, "echo "},
            {Litr::Script::TokenType::START_SEQ, "%{"},
            {Litr::Script::TokenType::IDENTIFIER, "case_camel"},
            {Litr::Script::TokenType::LEFT_PAREN, "("},
            {Litr::Script::TokenType::IDENTIFIER, "var"},
            {Litr::Script::TokenType::RIGHT_PAREN, ")"},
            {Litr::Script::TokenType::END_SEQ, "}"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Function usage with two arguments") {
    Litr::Script::Scanner scanner{"echo %{case_camel(var1, var2)}"};

    constexpr int item_count{9};
    std::array<TokenDefinition, item_count> definition{
        {{Litr::Script::TokenType::UNTOUCHED, "echo "},
            {Litr::Script::TokenType::START_SEQ, "%{"},
            {Litr::Script::TokenType::IDENTIFIER, "case_camel"},
            {Litr::Script::TokenType::LEFT_PAREN, "("},
            {Litr::Script::TokenType::IDENTIFIER, "var1"},
            {Litr::Script::TokenType::COMMA, ","},
            {Litr::Script::TokenType::IDENTIFIER, "var2"},
            {Litr::Script::TokenType::RIGHT_PAREN, ")"},
            {Litr::Script::TokenType::END_SEQ, "}"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Function usage with or-statement") {
    Litr::Script::Scanner scanner{"echo %{fn(var) 'First' or 'Second'}"};

    constexpr int item_count{10};
    std::array<TokenDefinition, item_count> definition{
        {{Litr::Script::TokenType::UNTOUCHED, "echo "},
            {Litr::Script::TokenType::START_SEQ, "%{"},
            {Litr::Script::TokenType::IDENTIFIER, "fn"},
            {Litr::Script::TokenType::LEFT_PAREN, "("},
            {Litr::Script::TokenType::IDENTIFIER, "var"},
            {Litr::Script::TokenType::RIGHT_PAREN, ")"},
            {Litr::Script::TokenType::STRING, "'First'"},
            {Litr::Script::TokenType::OR, "or"},
            {Litr::Script::TokenType::STRING, "'Second'"},
            {Litr::Script::TokenType::END_SEQ, "}"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Nested function usage") {
    Litr::Script::Scanner scanner{"echo %{fn1(fn2(var1), var2)}"};

    constexpr int item_count{12};
    std::array<TokenDefinition, item_count> definition{
        {{Litr::Script::TokenType::UNTOUCHED, "echo "},
            {Litr::Script::TokenType::START_SEQ, "%{"},
            {Litr::Script::TokenType::IDENTIFIER, "fn1"},
            {Litr::Script::TokenType::LEFT_PAREN, "("},
            {Litr::Script::TokenType::IDENTIFIER, "fn2"},
            {Litr::Script::TokenType::LEFT_PAREN, "("},
            {Litr::Script::TokenType::IDENTIFIER, "var1"},
            {Litr::Script::TokenType::RIGHT_PAREN, ")"},
            {Litr::Script::TokenType::COMMA, ","},
            {Litr::Script::TokenType::IDENTIFIER, "var2"},
            {Litr::Script::TokenType::RIGHT_PAREN, ")"},
            {Litr::Script::TokenType::END_SEQ, "}"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Does not care about invalid semantic") {
    Litr::Script::Scanner scanner{"echo %{var"};

    std::array<TokenDefinition, 3> definition{{{Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "var"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Ignores percentage sign without opening brace") {
    Litr::Script::Scanner scanner{"echo %var"};

    std::array<TokenDefinition, 1> definition{{{Litr::Script::TokenType::UNTOUCHED, "echo %var"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  // Error cases
  TEST_CASE("Unterminated string") {
    Litr::Script::Scanner scanner{"echo %{'Hello}"};

    std::array<TokenDefinition, 3> definition{{{Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::ERROR, "Unterminated string."}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Invalidates unknown characters, but does not stop scanning") {
    Litr::Script::Scanner scanner{"%{? fn}"};

    std::array<TokenDefinition, 4> definition{{
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::ERROR, "Unexpected character."},
        {Litr::Script::TokenType::IDENTIFIER, "fn"},
        {Litr::Script::TokenType::END_SEQ, "}"},
    }};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }
}
