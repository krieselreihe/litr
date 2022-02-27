/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/CLI/Parser.hpp"

#include <doctest/doctest.h>
#include <fmt/format.h>

#include <array>

#include "Core/Error/Handler.hpp"

#define NO_VALUE ""
#define CHECK_DEFINITION(instruction, definition)                                                \
  {                                                                                              \
    size_t iteration{0};                                                                         \
    size_t offset{0};                                                                            \
    while (offset < (instruction)->Count()) {                                                    \
      const auto test{(definition)[iteration]};                                                  \
      const auto code{static_cast<litr::CLI::Instruction::Code>((instruction)->Read(offset++))}; \
      switch (code) {                                                                            \
        case litr::CLI::Instruction::Code::CONSTANT:                                             \
        case litr::CLI::Instruction::Code::DEFINE:                                               \
        case litr::CLI::Instruction::Code::BEGIN_SCOPE:                                          \
        case litr::CLI::Instruction::Code::EXECUTE: {                                            \
          const std::byte index{(instruction)->Read(offset)};                                    \
          const litr::CLI::Instruction::Value constant{(instruction)->ReadConstant(index)};      \
          offset += 1;                                                                           \
          CHECK(test.Code == code);                                                              \
          CHECK(test.Value == constant);                                                         \
          break;                                                                                 \
        }                                                                                        \
        case litr::CLI::Instruction::Code::CLEAR: {                                              \
          CHECK(test.Code == code);                                                              \
          break;                                                                                 \
        }                                                                                        \
        default:                                                                                 \
          CHECK_MESSAGE(false, fmt::format("Unknown Instruction::Code {:d}", code));             \
          offset += 1;                                                                           \
      }                                                                                          \
      iteration++;                                                                               \
    }                                                                                            \
    CHECK(iteration == (definition).size());                                                     \
  }

struct InstructionDefinition {
  litr::CLI::Instruction::Code Code;
  litr::CLI::Instruction::Value Value;
};

TEST_SUITE("CLI::Parser") {
  TEST_CASE("Single long parameter") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{R"(--target="Some release")"};

    litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 2> definition{{
        {litr::CLI::Instruction::Code::DEFINE, "target"},
        {litr::CLI::Instruction::Code::CONSTANT, "Some release"}
    }};

    CHECK_FALSE(parser.HasErrors());
    CHECK_DEFINITION(instruction, definition);
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Single short parameter") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{R"(-t="debug is nice")"};

    litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 2> definition{{
        {litr::CLI::Instruction::Code::DEFINE, "t"},
        {litr::CLI::Instruction::Code::CONSTANT, "debug is nice"}
    }};

    CHECK_FALSE(parser.HasErrors());
    CHECK_DEFINITION(instruction, definition);
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Parameter with empty string") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{R"(-t="")"};

    litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 2> definition{{
        {litr::CLI::Instruction::Code::DEFINE, "t"},
        {litr::CLI::Instruction::Code::CONSTANT, ""}
    }};

    CHECK_FALSE(parser.HasErrors());
    CHECK_DEFINITION(instruction, definition);
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Single command") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{"build"};

    litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 2> definition{{
        {litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
        {litr::CLI::Instruction::Code::EXECUTE, "build"}
    }};

    CHECK_FALSE(parser.HasErrors());
    CHECK_DEFINITION(instruction, definition);
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Multiple commands") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{"build cpp"};

    litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 3> definition{{
        {litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
        {litr::CLI::Instruction::Code::BEGIN_SCOPE, "cpp"},
        {litr::CLI::Instruction::Code::EXECUTE, "build.cpp"}
    }};

    CHECK_FALSE(parser.HasErrors());
    CHECK_DEFINITION(instruction, definition);
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Multiple comma separated commands") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{"build,run"};

    litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 5> definition{{
        {litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
        {litr::CLI::Instruction::Code::EXECUTE, "build"},
        {litr::CLI::Instruction::Code::CLEAR, NO_VALUE},
        {litr::CLI::Instruction::Code::BEGIN_SCOPE, "run"},
        {litr::CLI::Instruction::Code::EXECUTE, "run"}
    }};

    CHECK_FALSE(parser.HasErrors());
    CHECK_DEFINITION(instruction, definition);
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Long parameter with string values and commands") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{R"(--target="release" build,run)"};

    litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 7> definition{{
        {litr::CLI::Instruction::Code::DEFINE, "target"},
        {litr::CLI::Instruction::Code::CONSTANT, "release"},
        {litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
        {litr::CLI::Instruction::Code::EXECUTE, "build"},
        {litr::CLI::Instruction::Code::CLEAR, NO_VALUE},
        {litr::CLI::Instruction::Code::BEGIN_SCOPE, "run"},
        {litr::CLI::Instruction::Code::EXECUTE, "run"}
    }};

    CHECK_FALSE(parser.HasErrors());
    CHECK_DEFINITION(instruction, definition);
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Mixed parameters with mixed values") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{R"(-t="release" --debug)"};

    litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 3> definition{{
        {litr::CLI::Instruction::Code::DEFINE, "t"},
        {litr::CLI::Instruction::Code::CONSTANT, "release"},
        {litr::CLI::Instruction::Code::DEFINE, "debug"}
    }};

    CHECK_FALSE(parser.HasErrors());
    CHECK_DEFINITION(instruction, definition);
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Invalid comma operator") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{","};

    litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `,`: Unexpected comma.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Invalid comma operators with parameter") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{R"(-t="debug" ,)"};

    litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors());

    const auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `,`: Unexpected comma.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Invalid comma operators with command") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{"run ,,"};

    litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors());

    const auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `,`: Duplicated comma.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Invalid multiple comma operators") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{", ,"};

    litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors());

    const auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `,`: Unexpected comma.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Invalid multiple comma operators with commands") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{"build cpp , ,"};

    litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors());

    const auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `,`: Duplicated comma.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Even more invalid multiple comma operators with commands") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{"build cpp ,,,,"};

    litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors());

    const auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `,`: Duplicated comma.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Invalid multiple comma operators with closing command") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{"build cpp , , run"};

    litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors());

    const auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `,`: Duplicated comma.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Valid nested command execution") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{R"(-t="debug" build cpp)"};

    litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 5> definition{{
        {litr::CLI::Instruction::Code::DEFINE, "t"},
        {litr::CLI::Instruction::Code::CONSTANT, "debug"},
        {litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
        {litr::CLI::Instruction::Code::BEGIN_SCOPE, "cpp"},
        {litr::CLI::Instruction::Code::EXECUTE, "build.cpp"}
    }};

    CHECK_FALSE(parser.HasErrors());
    CHECK_DEFINITION(instruction, definition);
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Valid multiple nested command execution") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{R"(-t="debug" build cpp, java)"};

    litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 8> definition{{
        {litr::CLI::Instruction::Code::DEFINE, "t"},
        {litr::CLI::Instruction::Code::CONSTANT, "debug"},
        {litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
        {litr::CLI::Instruction::Code::BEGIN_SCOPE, "cpp"},
        {litr::CLI::Instruction::Code::EXECUTE, "build.cpp"},
        {litr::CLI::Instruction::Code::CLEAR, NO_VALUE},
        {litr::CLI::Instruction::Code::BEGIN_SCOPE, "java"},
        {litr::CLI::Instruction::Code::EXECUTE, "build.java"}
    }};

    CHECK_FALSE(parser.HasErrors());
    CHECK_DEFINITION(instruction, definition);
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Unsupported characters") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{"(9)"};

    litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors());

    const auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse: Unexpected character.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Unsupported string between commands") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{R"(build "debug" project )"};

    litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors());

    const auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `\"debug\"`: This is not allowed here.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Unsupported number between commands") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{R"(build 23 project )"};

    litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors());

    const auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `23`: This is not allowed here.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Unsupported characters between commands") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{R"(build ++ project )"};

    litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors());

    const auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse: Unexpected character.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Assignment missing parameter") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{R"(= "value")"};

    litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors());

    const auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `=`: You are missing a parameter in front of the assignment.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Duplicated assignment operator") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{R"(-t == "value")"};

    litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors());

    const auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse at `=`: Value assignment missing.");
    litr::Error::Handler::Flush();
  }

  TEST_CASE("Duplicated parameter initializer") {
    const auto instruction{litr::CreateRef<litr::CLI::Instruction>()};
    const std::string source{"---t"};

    litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors());

    const auto errors{litr::Error::Handler::GetErrors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].Message, "Cannot parse: A parameter can only start with the characters A-Za-z.");
    litr::Error::Handler::Flush();
  }
}
