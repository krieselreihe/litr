/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include "Core/CLI/Parser.hpp"

#include <doctest/doctest.h>
#include <fmt/format.h>

#include <array>
#include <memory>

#include "Core/Error/Handler.hpp"

#define NO_VALUE ""
#define CHECK_DEFINITION(instruction, definition)                                            \
  {                                                                                          \
    size_t iteration{0};                                                                     \
    size_t offset{0};                                                                        \
    while (offset < (instruction)->count()) {                                                \
      const auto test{(definition)[iteration]};                                              \
      const litr::cli::Instruction::Code code{(instruction)->read(offset++)};                \
      switch (code) {                                                                        \
        case litr::cli::Instruction::Code::CONSTANT:                                         \
        case litr::cli::Instruction::Code::DEFINE:                                           \
        case litr::cli::Instruction::Code::BEGIN_SCOPE:                                      \
        case litr::cli::Instruction::Code::EXECUTE: {                                        \
          const std::byte index{(instruction)->read(offset)};                                \
          const litr::cli::Instruction::Value constant{(instruction)->read_constant(index)}; \
          offset += 1;                                                                       \
          CHECK_EQ(test.code, code);                                                         \
          CHECK_EQ(test.value, constant);                                                    \
          break;                                                                             \
        }                                                                                    \
        case litr::cli::Instruction::Code::CLEAR: {                                          \
          CHECK_EQ(test.code, code);                                                         \
          break;                                                                             \
        }                                                                                    \
        default:                                                                             \
          CHECK_MESSAGE(false, fmt::format("Unknown Instruction::Code {:d}", code));         \
          offset += 1;                                                                       \
      }                                                                                      \
      ++iteration;                                                                           \
    }                                                                                        \
    CHECK_EQ(iteration, (definition).size());                                                \
  }

struct InstructionDefinition {
  litr::cli::Instruction::Code code;
  litr::cli::Instruction::Value value;
};

TEST_SUITE("CLI::Parser") {
  TEST_CASE("Single long parameter") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{R"(--target="Some release")"};

    litr::cli::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 2> definition{
        {{litr::cli::Instruction::Code::DEFINE, "target"},
            {litr::cli::Instruction::Code::CONSTANT, "Some release"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    litr::error::Handler::flush();
  }

  TEST_CASE("Single short parameter") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{R"(-t="debug is nice")"};

    litr::cli::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 2> definition{
        {{litr::cli::Instruction::Code::DEFINE, "t"},
            {litr::cli::Instruction::Code::CONSTANT, "debug is nice"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    litr::error::Handler::flush();
  }

  TEST_CASE("Parameter with empty string") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{R"(-t="")"};

    litr::cli::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 2> definition{
        {{litr::cli::Instruction::Code::DEFINE, "t"},
            {litr::cli::Instruction::Code::CONSTANT, ""}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    litr::error::Handler::flush();
  }

  TEST_CASE("Single command") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{"build"};

    litr::cli::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 2> definition{
        {{litr::cli::Instruction::Code::BEGIN_SCOPE, "build"},
            {litr::cli::Instruction::Code::EXECUTE, "build"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    litr::error::Handler::flush();
  }

  TEST_CASE("Multiple commands") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{"build cpp"};

    litr::cli::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 3> definition{
        {{litr::cli::Instruction::Code::BEGIN_SCOPE, "build"},
            {litr::cli::Instruction::Code::BEGIN_SCOPE, "cpp"},
            {litr::cli::Instruction::Code::EXECUTE, "build.cpp"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    litr::error::Handler::flush();
  }

  TEST_CASE("Multiple comma separated commands") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{"build,run"};

    litr::cli::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 5> definition{
        {{litr::cli::Instruction::Code::BEGIN_SCOPE, "build"},
            {litr::cli::Instruction::Code::EXECUTE, "build"},
            {litr::cli::Instruction::Code::CLEAR, NO_VALUE},
            {litr::cli::Instruction::Code::BEGIN_SCOPE, "run"},
            {litr::cli::Instruction::Code::EXECUTE, "run"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    litr::error::Handler::flush();
  }

  TEST_CASE("Long parameter with string values and commands") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{R"(--target="release" build,run)"};

    litr::cli::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 7> definition{
        {{litr::cli::Instruction::Code::DEFINE, "target"},
            {litr::cli::Instruction::Code::CONSTANT, "release"},
            {litr::cli::Instruction::Code::BEGIN_SCOPE, "build"},
            {litr::cli::Instruction::Code::EXECUTE, "build"},
            {litr::cli::Instruction::Code::CLEAR, NO_VALUE},
            {litr::cli::Instruction::Code::BEGIN_SCOPE, "run"},
            {litr::cli::Instruction::Code::EXECUTE, "run"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    litr::error::Handler::flush();
  }

  TEST_CASE("Mixed parameters with mixed values") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{R"(-t="release" --debug)"};

    litr::cli::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 3> definition{
        {{litr::cli::Instruction::Code::DEFINE, "t"},
            {litr::cli::Instruction::Code::CONSTANT, "release"},
            {litr::cli::Instruction::Code::DEFINE, "debug"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    litr::error::Handler::flush();
  }

  TEST_CASE("Invalid comma operator") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{","};

    litr::cli::Parser parser{instruction, source};

    CHECK(parser.has_errors() == true);

    const auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Unexpected comma.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Invalid comma operators with parameter") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{R"(-t="debug" ,)"};

    litr::cli::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Unexpected comma.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Invalid comma operators with command") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{"run ,,"};

    litr::cli::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Duplicated comma.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Invalid multiple comma operators") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{", ,"};

    litr::cli::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Unexpected comma.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Invalid multiple comma operators with commands") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{"build cpp , ,"};

    litr::cli::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Duplicated comma.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Even more invalid multiple comma operators with commands") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{"build cpp ,,,,"};

    litr::cli::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Duplicated comma.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Invalid multiple comma operators with closing command") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{"build cpp , , run"};

    litr::cli::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Duplicated comma.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Valid nested command execution") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{R"(-t="debug" build cpp)"};

    litr::cli::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 5> definition{
        {{litr::cli::Instruction::Code::DEFINE, "t"},
            {litr::cli::Instruction::Code::CONSTANT, "debug"},
            {litr::cli::Instruction::Code::BEGIN_SCOPE, "build"},
            {litr::cli::Instruction::Code::BEGIN_SCOPE, "cpp"},
            {litr::cli::Instruction::Code::EXECUTE, "build.cpp"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    litr::error::Handler::flush();
  }

  TEST_CASE("Valid multiple nested command execution") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{R"(-t="debug" build cpp, java)"};

    litr::cli::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 8> definition{
        {{litr::cli::Instruction::Code::DEFINE, "t"},
            {litr::cli::Instruction::Code::CONSTANT, "debug"},
            {litr::cli::Instruction::Code::BEGIN_SCOPE, "build"},
            {litr::cli::Instruction::Code::BEGIN_SCOPE, "cpp"},
            {litr::cli::Instruction::Code::EXECUTE, "build.cpp"},
            {litr::cli::Instruction::Code::CLEAR, NO_VALUE},
            {litr::cli::Instruction::Code::BEGIN_SCOPE, "java"},
            {litr::cli::Instruction::Code::EXECUTE, "build.java"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    litr::error::Handler::flush();
  }

  TEST_CASE("Unsupported characters") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{"(9)"};

    litr::cli::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse: Unexpected character.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Unsupported string between commands") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{R"(build "debug" project )"};

    litr::cli::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `\"debug\"`: This is not allowed here.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Unsupported number between commands") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{R"(build 23 project )"};

    litr::cli::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `23`: This is not allowed here.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Unsupported characters between commands") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{R"(build ++ project )"};

    litr::cli::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse: Unexpected character.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Assignment missing parameter") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{R"(= "value")"};

    litr::cli::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message,
        "Cannot parse at `=`: You are missing a parameter in front of the assignment.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Duplicated assignment operator") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{R"(-t == "value")"};

    litr::cli::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `=`: Value assignment missing.");
    litr::error::Handler::flush();
  }

  TEST_CASE("Duplicated parameter initializer") {
    const auto instruction{std::make_shared<litr::cli::Instruction>()};
    const std::string source{"---t"};

    litr::cli::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{litr::error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(
        errors[0].message, "Cannot parse: A parameter can only start with the characters A-Za-z.");
    litr::error::Handler::flush();
  }
}
