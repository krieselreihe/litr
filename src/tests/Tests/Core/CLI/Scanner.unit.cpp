/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include <doctest/doctest.h>

#include <array>

// Not sure why, but doctest really wants iostream here when
// using g++. Who am I to judge.
#include <iostream>

#include "Core/CLI/Scanner.hpp"

#define CHECK_DEFINITION(scanner, definition)                           \
  {                                                                     \
    for (auto&& test : (definition)) {                                  \
      litr::cli::Token token{(scanner).scan_token()};                   \
      CHECK_EQ(token.type, test.type);                                  \
      CHECK_EQ(litr::cli::Scanner::get_token_value(token), test.value); \
    }                                                                   \
  }

#define CHECK_EOS_TOKEN(scanner)                            \
  {                                                         \
    litr::cli::Token eos{(scanner).scan_token()};           \
    CHECK_EQ(eos.type, litr::cli::TokenType::EOS);          \
    CHECK_EQ(litr::cli::Scanner::get_token_value(eos), ""); \
  }

struct TokenDefinition {
  litr::cli::TokenType type;
  std::string value;
};

TEST_SUITE("CLI::Scanner") {
  // Successful cases
  TEST_CASE("Single long parameter") {
    litr::cli::Scanner scanner{"--help"};

    std::array<TokenDefinition, 1> definition{{{litr::cli::TokenType::LONG_PARAMETER, "--help"}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single short parameter") {
    litr::cli::Scanner scanner{"-h"};

    std::array<TokenDefinition, 1> definition{{{litr::cli::TokenType::SHORT_PARAMETER, "-h"}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single command") {
    litr::cli::Scanner scanner{"build"};

    std::array<TokenDefinition, 1> definition{{{litr::cli::TokenType::COMMAND, "build"}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple commands") {
    litr::cli::Scanner scanner{"build cpp"};

    std::array<TokenDefinition, 2> definition{{
        {litr::cli::TokenType::COMMAND, "build"},
        {litr::cli::TokenType::COMMAND, "cpp"},
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("More multiple commands with whitespace") {
    litr::cli::Scanner scanner{"build cpp   another"};

    std::array<TokenDefinition, 3> definition{{{litr::cli::TokenType::COMMAND, "build"},
        {litr::cli::TokenType::COMMAND, "cpp"},
        {litr::cli::TokenType::COMMAND, "another"}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple comma separated commands") {
    litr::cli::Scanner scanner{"build,run"};

    std::array<TokenDefinition, 3> definition{{{litr::cli::TokenType::COMMAND, "build"},
        {litr::cli::TokenType::COMMA, ","},
        {litr::cli::TokenType::COMMAND, "run"}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple comma separated commands with whitespace") {
    litr::cli::Scanner scanner{"build ,  run"};

    std::array<TokenDefinition, 3> definition{{{litr::cli::TokenType::COMMAND, "build"},
        {litr::cli::TokenType::COMMA, ","},
        {litr::cli::TokenType::COMMAND, "run"}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Long parameter with string values") {
    litr::cli::Scanner scanner{" --target=\"release value\" build,run"};

    std::array<TokenDefinition, 6> definition{{{litr::cli::TokenType::LONG_PARAMETER, "--target"},
        {litr::cli::TokenType::EQUAL, "="},
        {litr::cli::TokenType::STRING, "\"release value\""},
        {litr::cli::TokenType::COMMAND, "build"},
        {litr::cli::TokenType::COMMA, ","},
        {litr::cli::TokenType::COMMAND, "run"}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Long parameter with string values with whitespace") {
    litr::cli::Scanner scanner{"  --target  = \"release\"  build ,run  "};

    std::array<TokenDefinition, 6> definition{{{litr::cli::TokenType::LONG_PARAMETER, "--target"},
        {litr::cli::TokenType::EQUAL, "="},
        {litr::cli::TokenType::STRING, "\"release\""},
        {litr::cli::TokenType::COMMAND, "build"},
        {litr::cli::TokenType::COMMA, ","},
        {litr::cli::TokenType::COMMAND, "run"}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Short parameter with number values") {
    litr::cli::Scanner scanner{" -t=42"};

    std::array<TokenDefinition, 3> definition{{{litr::cli::TokenType::SHORT_PARAMETER, "-t"},
        {litr::cli::TokenType::EQUAL, "="},
        {litr::cli::TokenType::NUMBER, "42"}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Mixed parameters with mixed values and whitespace") {
    litr::cli::Scanner scanner{" -t=\"release\" --debug  -p = 2.45"};

    std::array<TokenDefinition, 7> definition{{{litr::cli::TokenType::SHORT_PARAMETER, "-t"},
        {litr::cli::TokenType::EQUAL, "="},
        {litr::cli::TokenType::STRING, "\"release\""},
        {litr::cli::TokenType::LONG_PARAMETER, "--debug"},
        {litr::cli::TokenType::SHORT_PARAMETER, "-p"},
        {litr::cli::TokenType::EQUAL, "="},
        {litr::cli::TokenType::NUMBER, "2.45"}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Mixed source string") {
    litr::cli::Scanner scanner{"-t=\"release\" build, run --peer = 1.23"};

    std::array<TokenDefinition, 9> definition{{{litr::cli::TokenType::SHORT_PARAMETER, "-t"},
        {litr::cli::TokenType::EQUAL, "="},
        {litr::cli::TokenType::STRING, "\"release\""},
        {litr::cli::TokenType::COMMAND, "build"},
        {litr::cli::TokenType::COMMA, ","},
        {litr::cli::TokenType::COMMAND, "run"},
        {litr::cli::TokenType::LONG_PARAMETER, "--peer"},
        {litr::cli::TokenType::EQUAL, "="},
        {litr::cli::TokenType::NUMBER, "1.23"}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Does not care about invalid semantics") {
    litr::cli::Scanner scanner{", , ,"};

    std::array<TokenDefinition, 3> definition{{
        {litr::cli::TokenType::COMMA, ","},
        {litr::cli::TokenType::COMMA, ","},
        {litr::cli::TokenType::COMMA, ","},
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  // Error cases
  TEST_CASE("Invalidates unterminated string syntax") {
    litr::cli::Scanner scanner{"\"str"};

    std::array<TokenDefinition, 1> definition{
        {{litr::cli::TokenType::ERROR, "Unterminated string."}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates unknown characters, but does not stop scanning") {
    litr::cli::Scanner scanner{"? -p"};

    std::array<TokenDefinition, 2> definition{
        {{litr::cli::TokenType::ERROR, "Unexpected character."},
            {litr::cli::TokenType::SHORT_PARAMETER, "-p"}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates wrong long parameter start character") {
    litr::cli::Scanner scanner{"--1 --_bob"};

    std::array<TokenDefinition, 2> definition{
        {{litr::cli::TokenType::ERROR, "A parameter can only start with the characters A-Za-z."},
            {litr::cli::TokenType::ERROR,
                "A parameter can only start with the characters A-Za-z."}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates wrong short parameter name") {
    litr::cli::Scanner scanner{"-1 -_ -?"};

    std::array<TokenDefinition, 3> definition{
        {{litr::cli::TokenType::ERROR, "A short parameter can only be A-Za-z as name."},
            {litr::cli::TokenType::ERROR, "A short parameter can only be A-Za-z as name."},
            {litr::cli::TokenType::ERROR, "A short parameter can only be A-Za-z as name."}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates too long short parameter name") {
    litr::cli::Scanner scanner{"-long"};

    std::array<TokenDefinition, 1> definition{{{litr::cli::TokenType::ERROR,
        "A short parameter can only contain one character (A-Za-z)."}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates duplicated parameter initializer") {
    litr::cli::Scanner scanner{"---long"};

    std::array<TokenDefinition, 1> definition{
        {{litr::cli::TokenType::ERROR, "A parameter can only start with the characters A-Za-z."}}};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }
}
