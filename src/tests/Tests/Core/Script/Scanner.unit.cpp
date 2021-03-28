#include <doctest/doctest.h>
#include <array>

// Not sure why, but doctest really wants iostream here when
// using g++. Who am I to judge.
#include <iostream>

#include "Core/Script/Scanner.hpp"

#define CHECK_DEFINITION(scanner, definition)                           \
  {                                                                     \
    for (auto&& test : (definition)) {                                  \
      Litr::Script::Token token{(scanner).ScanToken()};                 \
      CHECK(token.Type == test.Type);                                   \
      CHECK(Litr::Script::Scanner::GetTokenValue(token) == test.Value); \
    }                                                                   \
  }

#define CHECK_EOS_TOKEN(scanner)                            \
  {                                                         \
    Litr::Script::Token eos{(scanner).ScanToken()};         \
    CHECK(eos.Type == Litr::Script::TokenType::EOS);        \
    CHECK(Litr::Script::Scanner::GetTokenValue(eos) == ""); \
  }

struct TokenDefinition {
  Litr::Script::TokenType Type;
  std::string Value;
} __attribute__((aligned(32)));

TEST_SUITE("Scanner") {
  // Successful cases
  TEST_CASE("Nothing to parse") {
    Litr::Script::Scanner scanner{"echo 'Hello World!'"};

    std::array<TokenDefinition, 1> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo 'Hello World!'"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Nothing to parse on escaped sequence") {
    Litr::Script::Scanner scanner{R"(echo \%{something})"};

    std::array<TokenDefinition, 1> definition{{
        {Litr::Script::TokenType::UNTOUCHED, R"(echo \%{something})"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single variable") {
    Litr::Script::Scanner scanner{"echo %{var}"};

    std::array<TokenDefinition, 4> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "var"},
        {Litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Ignores duplicated closing brace") {
    Litr::Script::Scanner scanner{"echo %{var}}"};

    std::array<TokenDefinition, 5> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "var"},
        {Litr::Script::TokenType::END_SEQ, "}"},
        {Litr::Script::TokenType::UNTOUCHED, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple variables") {
    Litr::Script::Scanner scanner{"echo %{var1} and %{var2}"};

    std::array<TokenDefinition, 8> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "var1"},
        {Litr::Script::TokenType::END_SEQ, "}"},
        {Litr::Script::TokenType::UNTOUCHED, " and "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "var2"},
        {Litr::Script::TokenType::END_SEQ, "}"},
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Multiple variables in between") {
    Litr::Script::Scanner scanner{"echo %{var1} and %{var2} end"};

    std::array<TokenDefinition, 9> definition{{
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

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single string") {
    Litr::Script::Scanner scanner{"echo %{'Hello'}"};

    std::array<TokenDefinition, 4> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::STRING, "'Hello'"},
        {Litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single string including ignored expression syntax") {
    Litr::Script::Scanner scanner{"echo %{'He%{ll}o'}"};

    std::array<TokenDefinition, 4> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::STRING, "'He%{ll}o'"},
        {Litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Single string with leading variable") {
    Litr::Script::Scanner scanner{"echo %{var 'Hello'}"};

    std::array<TokenDefinition, 5> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "var"},
        {Litr::Script::TokenType::STRING, "'Hello'"},
        {Litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Strings and or-statement") {
    Litr::Script::Scanner scanner{"echo %{var 'Hello' or 'Bye'}"};

    std::array<TokenDefinition, 7> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "var"},
        {Litr::Script::TokenType::STRING, "'Hello'"},
        {Litr::Script::TokenType::OR, "or"},
        {Litr::Script::TokenType::STRING, "'Bye'"},
        {Litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  /*
  TEST_CASE("Strings and and-statement") {
    Litr::Script::Scanner scanner{"echo %{var1 and var2 'Hello'}"};

    std::array<TokenDefinition, 7> definition{{
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

    std::array<TokenDefinition, 7> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "case_camel"},
        {Litr::Script::TokenType::LEFT_PAREN, "("},
        {Litr::Script::TokenType::IDENTIFIER, "var"},
        {Litr::Script::TokenType::RIGHT_PAREN, ")"},
        {Litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Function usage with two arguments") {
    Litr::Script::Scanner scanner{"echo %{case_camel(var1, var2)}"};

    std::array<TokenDefinition, 9> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "case_camel"},
        {Litr::Script::TokenType::LEFT_PAREN, "("},
        {Litr::Script::TokenType::IDENTIFIER, "var1"},
        {Litr::Script::TokenType::COMMA, ","},
        {Litr::Script::TokenType::IDENTIFIER, "var2"},
        {Litr::Script::TokenType::RIGHT_PAREN, ")"},
        {Litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Function usage with or-statement") {
    Litr::Script::Scanner scanner{"echo %{fn(var) 'First' or 'Second'}"};

    std::array<TokenDefinition, 10> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "fn"},
        {Litr::Script::TokenType::LEFT_PAREN, "("},
        {Litr::Script::TokenType::IDENTIFIER, "var"},
        {Litr::Script::TokenType::RIGHT_PAREN, ")"},
        {Litr::Script::TokenType::STRING, "'First'"},
        {Litr::Script::TokenType::OR, "or"},
        {Litr::Script::TokenType::STRING, "'Second'"},
        {Litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Nested function usage") {
    Litr::Script::Scanner scanner{"echo %{fn1(fn2(var1), var2)}"};

    std::array<TokenDefinition, 12> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo "},
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
        {Litr::Script::TokenType::END_SEQ, "}"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Does not care about invalid semantic") {
    Litr::Script::Scanner scanner{"echo %{var"};

    std::array<TokenDefinition, 3> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::IDENTIFIER, "var"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Ignores percentage sign without opening brace") {
    Litr::Script::Scanner scanner{"echo %var"};

    std::array<TokenDefinition, 1> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo %var"}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  // Error cases
  TEST_CASE("Unterminated string") {
    Litr::Script::Scanner scanner{"echo %{'Hello}"};

    std::array<TokenDefinition, 3> definition{{
        {Litr::Script::TokenType::UNTOUCHED, "echo "},
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::ERROR, "Unterminated string."}
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }

  TEST_CASE("Invalidates unknown characters, but does not stop scanning") {
    Litr::Script::Scanner scanner{"%{? fn}"};

    std::array<TokenDefinition, 4> definition{{
        {Litr::Script::TokenType::START_SEQ, "%{"},
        {Litr::Script::TokenType::ERROR, "Unexpected character."},
        {Litr::Script::TokenType::IDENTIFIER, "fn"},
        {Litr::Script::TokenType::END_SEQ, "}"},
    }};

    CHECK_DEFINITION(scanner, definition);
    CHECK_EOS_TOKEN(scanner);
  }
}
