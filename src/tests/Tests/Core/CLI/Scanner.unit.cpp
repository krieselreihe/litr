/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/CLI/Scanner.hpp"

#include <doctest/doctest.h>

#include <array>

struct TokenDefinition {
  Litr::CLI::TokenType type;
  std::string value;
};

template <typename T, size_t S>
inline void check_definition(Litr::CLI::Scanner& scanner, const std::array<T, S>& definition) {
  for (auto&& test : definition) {
    Litr::CLI::Token token{scanner.scan_token()};
    // NOLINTNEXTLINE
    CHECK_EQ(token.type, test.type);
    // NOLINTNEXTLINE
    CHECK_EQ(Litr::CLI::Scanner::get_token_value(token), test.value);
  }
}

inline void check_eos_token(Litr::CLI::Scanner& scanner) {
  Litr::CLI::Token eos{(scanner).scan_token()};
  // NOLINTNEXTLINE
  CHECK_EQ(eos.type, Litr::CLI::TokenType::EOS);
  // NOLINTNEXTLINE
  CHECK_EQ(Litr::CLI::Scanner::get_token_value(eos), "");
}

TEST_SUITE("CLI::Scanner") {
  // Successful cases
  TEST_CASE("Single long parameter") {
    Litr::CLI::Scanner scanner{"--help"};

    std::array<TokenDefinition, 1> definition{{{Litr::CLI::TokenType::LONG_PARAMETER, "--help"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Single short parameter") {
    Litr::CLI::Scanner scanner{"-h"};

    std::array<TokenDefinition, 1> definition{{{Litr::CLI::TokenType::SHORT_PARAMETER, "-h"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Single command") {
    Litr::CLI::Scanner scanner{"build"};

    std::array<TokenDefinition, 1> definition{{{Litr::CLI::TokenType::COMMAND, "build"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Multiple commands") {
    Litr::CLI::Scanner scanner{"build cpp"};

    std::array<TokenDefinition, 2> definition{{
        {Litr::CLI::TokenType::COMMAND, "build"},
        {Litr::CLI::TokenType::COMMAND, "cpp"},
    }};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("More multiple commands with whitespace") {
    Litr::CLI::Scanner scanner{"build cpp   another"};

    std::array<TokenDefinition, 3> definition{{{Litr::CLI::TokenType::COMMAND, "build"},
        {Litr::CLI::TokenType::COMMAND, "cpp"},
        {Litr::CLI::TokenType::COMMAND, "another"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Multiple comma separated commands") {
    Litr::CLI::Scanner scanner{"build,run"};

    std::array<TokenDefinition, 3> definition{{{Litr::CLI::TokenType::COMMAND, "build"},
        {Litr::CLI::TokenType::COMMA, ","},
        {Litr::CLI::TokenType::COMMAND, "run"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Multiple comma separated commands with whitespace") {
    Litr::CLI::Scanner scanner{"build ,  run"};

    std::array<TokenDefinition, 3> definition{{{Litr::CLI::TokenType::COMMAND, "build"},
        {Litr::CLI::TokenType::COMMA, ","},
        {Litr::CLI::TokenType::COMMAND, "run"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Long parameter with string values") {
    Litr::CLI::Scanner scanner{" --target=\"release value\" build,run"};

    constexpr int item_count{6};
    std::array<TokenDefinition, item_count> definition{
        {{Litr::CLI::TokenType::LONG_PARAMETER, "--target"},
            {Litr::CLI::TokenType::EQUAL, "="},
            {Litr::CLI::TokenType::STRING, "\"release value\""},
            {Litr::CLI::TokenType::COMMAND, "build"},
            {Litr::CLI::TokenType::COMMA, ","},
            {Litr::CLI::TokenType::COMMAND, "run"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Long parameter with string values with whitespace") {
    Litr::CLI::Scanner scanner{"  --target  = \"release\"  build ,run  "};

    constexpr int item_count{6};
    std::array<TokenDefinition, item_count> definition{
        {{Litr::CLI::TokenType::LONG_PARAMETER, "--target"},
            {Litr::CLI::TokenType::EQUAL, "="},
            {Litr::CLI::TokenType::STRING, "\"release\""},
            {Litr::CLI::TokenType::COMMAND, "build"},
            {Litr::CLI::TokenType::COMMA, ","},
            {Litr::CLI::TokenType::COMMAND, "run"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Short parameter with number values") {
    Litr::CLI::Scanner scanner{" -t=42"};

    std::array<TokenDefinition, 3> definition{{{Litr::CLI::TokenType::SHORT_PARAMETER, "-t"},
        {Litr::CLI::TokenType::EQUAL, "="},
        {Litr::CLI::TokenType::NUMBER, "42"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Mixed parameters with mixed values and whitespace") {
    Litr::CLI::Scanner scanner{" -t=\"release\" --debug  -p = 2.45"};

    constexpr int item_count{7};
    std::array<TokenDefinition, item_count> definition{
        {{Litr::CLI::TokenType::SHORT_PARAMETER, "-t"},
            {Litr::CLI::TokenType::EQUAL, "="},
            {Litr::CLI::TokenType::STRING, "\"release\""},
            {Litr::CLI::TokenType::LONG_PARAMETER, "--debug"},
            {Litr::CLI::TokenType::SHORT_PARAMETER, "-p"},
            {Litr::CLI::TokenType::EQUAL, "="},
            {Litr::CLI::TokenType::NUMBER, "2.45"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Mixed source string") {
    Litr::CLI::Scanner scanner{"-t=\"release\" build, run --peer = 1.23"};

    constexpr int item_count{9};
    std::array<TokenDefinition, item_count> definition{
        {{Litr::CLI::TokenType::SHORT_PARAMETER, "-t"},
            {Litr::CLI::TokenType::EQUAL, "="},
            {Litr::CLI::TokenType::STRING, "\"release\""},
            {Litr::CLI::TokenType::COMMAND, "build"},
            {Litr::CLI::TokenType::COMMA, ","},
            {Litr::CLI::TokenType::COMMAND, "run"},
            {Litr::CLI::TokenType::LONG_PARAMETER, "--peer"},
            {Litr::CLI::TokenType::EQUAL, "="},
            {Litr::CLI::TokenType::NUMBER, "1.23"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Does not care about invalid semantics") {
    Litr::CLI::Scanner scanner{", , ,"};

    std::array<TokenDefinition, 3> definition{{
        {Litr::CLI::TokenType::COMMA, ","},
        {Litr::CLI::TokenType::COMMA, ","},
        {Litr::CLI::TokenType::COMMA, ","},
    }};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  // Error cases
  TEST_CASE("Invalidates unterminated string syntax") {
    Litr::CLI::Scanner scanner{"\"str"};

    std::array<TokenDefinition, 1> definition{
        {{Litr::CLI::TokenType::ERROR, "Unterminated string."}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Invalidates unknown characters, but does not stop scanning") {
    Litr::CLI::Scanner scanner{"? -p"};

    std::array<TokenDefinition, 2> definition{
        {{Litr::CLI::TokenType::ERROR, "Unexpected character."},
            {Litr::CLI::TokenType::SHORT_PARAMETER, "-p"}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Invalidates wrong long parameter start character") {
    Litr::CLI::Scanner scanner{"--1 --_bob"};

    std::array<TokenDefinition, 2> definition{
        {{Litr::CLI::TokenType::ERROR, "A parameter can only start with the characters A-Za-z."},
            {Litr::CLI::TokenType::ERROR,
                "A parameter can only start with the characters A-Za-z."}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Invalidates wrong short parameter name") {
    Litr::CLI::Scanner scanner{"-1 -_ -?"};

    std::array<TokenDefinition, 3> definition{
        {{Litr::CLI::TokenType::ERROR, "A short parameter can only be A-Za-z as name."},
            {Litr::CLI::TokenType::ERROR, "A short parameter can only be A-Za-z as name."},
            {Litr::CLI::TokenType::ERROR, "A short parameter can only be A-Za-z as name."}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Invalidates too long short parameter name") {
    Litr::CLI::Scanner scanner{"-long"};

    std::array<TokenDefinition, 1> definition{{{Litr::CLI::TokenType::ERROR,
        "A short parameter can only contain one character (A-Za-z)."}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }

  TEST_CASE("Invalidates duplicated parameter initializer") {
    Litr::CLI::Scanner scanner{"---long"};

    std::array<TokenDefinition, 1> definition{
        {{Litr::CLI::TokenType::ERROR, "A parameter can only start with the characters A-Za-z."}}};

    check_definition(scanner, definition);
    check_eos_token(scanner);
  }
}
