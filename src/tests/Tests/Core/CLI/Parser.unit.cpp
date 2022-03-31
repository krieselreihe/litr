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
#define CHECK_DEFINITION(instruction, definition)                                               \
  {                                                                                             \
    size_t iteration{0};                                                                        \
    size_t offset{0};                                                                           \
    while (offset < (instruction)->count()) {                                                   \
      const auto test{(definition)[iteration]};                                                 \
      const Litr::CLI::Instruction::Code code{(instruction)->read(offset++)};                   \
      switch (code) {                                                                           \
        case Litr::CLI::Instruction::Code::CONSTANT:                                            \
        case Litr::CLI::Instruction::Code::DEFINE:                                              \
        case Litr::CLI::Instruction::Code::BEGIN_SCOPE:                                         \
        case Litr::CLI::Instruction::Code::EXECUTE: {                                           \
          const std::byte index{(instruction)->read(offset)};                                   \
          const Litr::CLI::Instruction::Value constant{(instruction)->read_constant(index)};    \
          offset += 1;                                                                          \
          CHECK_EQ(test.code, code);                                                            \
          CHECK_EQ(test.value, constant);                                                       \
          break;                                                                                \
        }                                                                                       \
        case Litr::CLI::Instruction::Code::CLEAR: {                                             \
          CHECK_EQ(test.code, code);                                                            \
          break;                                                                                \
        }                                                                                       \
        default:                                                                                \
          CHECK_MESSAGE(false,                                                                  \
              fmt::format("Unknown Instruction::Code {:d}", static_cast<unsigned char>(code))); \
          offset += 1;                                                                          \
      }                                                                                         \
      ++iteration;                                                                              \
    }                                                                                           \
    CHECK_EQ(iteration, (definition).size());                                                   \
  }

struct InstructionDefinition {
  Litr::CLI::Instruction::Code code;
  Litr::CLI::Instruction::Value value;
};

TEST_SUITE("CLI::Parser") {
  TEST_CASE("Single long parameter") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{R"(--target="Some release")"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 2> definition{
        {{Litr::CLI::Instruction::Code::DEFINE, "target"},
            {Litr::CLI::Instruction::Code::CONSTANT, "Some release"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Single short parameter") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{R"(-t="debug is nice")"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 2> definition{
        {{Litr::CLI::Instruction::Code::DEFINE, "t"},
            {Litr::CLI::Instruction::Code::CONSTANT, "debug is nice"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Parameter with empty string") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{R"(-t="")"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 2> definition{
        {{Litr::CLI::Instruction::Code::DEFINE, "t"},
            {Litr::CLI::Instruction::Code::CONSTANT, ""}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Single command") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{"build"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 2> definition{
        {{Litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
            {Litr::CLI::Instruction::Code::EXECUTE, "build"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Multiple commands") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{"build cpp"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 3> definition{
        {{Litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
            {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "cpp"},
            {Litr::CLI::Instruction::Code::EXECUTE, "build.cpp"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Multiple comma separated commands") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{"build,run"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 5> definition{
        {{Litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
            {Litr::CLI::Instruction::Code::EXECUTE, "build"},
            {Litr::CLI::Instruction::Code::CLEAR, NO_VALUE},
            {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "run"},
            {Litr::CLI::Instruction::Code::EXECUTE, "run"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Long parameter with string values and commands") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{R"(--target="release" build,run)"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 7> definition{
        {{Litr::CLI::Instruction::Code::DEFINE, "target"},
            {Litr::CLI::Instruction::Code::CONSTANT, "release"},
            {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
            {Litr::CLI::Instruction::Code::EXECUTE, "build"},
            {Litr::CLI::Instruction::Code::CLEAR, NO_VALUE},
            {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "run"},
            {Litr::CLI::Instruction::Code::EXECUTE, "run"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Mixed parameters with mixed values") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{R"(-t="release" --debug)"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 3> definition{
        {{Litr::CLI::Instruction::Code::DEFINE, "t"},
            {Litr::CLI::Instruction::Code::CONSTANT, "release"},
            {Litr::CLI::Instruction::Code::DEFINE, "debug"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Invalid comma operator") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{","};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.has_errors() == true);

    const auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Unexpected comma.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Invalid comma operators with parameter") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{R"(-t="debug" ,)"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Unexpected comma.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Invalid comma operators with command") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{"run ,,"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Duplicated comma.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Invalid multiple comma operators") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{", ,"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Unexpected comma.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Invalid multiple comma operators with commands") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{"build cpp , ,"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Duplicated comma.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Even more invalid multiple comma operators with commands") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{"build cpp ,,,,"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Duplicated comma.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Invalid multiple comma operators with closing command") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{"build cpp , , run"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `,`: Duplicated comma.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Valid nested command execution") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{R"(-t="debug" build cpp)"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 5> definition{
        {{Litr::CLI::Instruction::Code::DEFINE, "t"},
            {Litr::CLI::Instruction::Code::CONSTANT, "debug"},
            {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
            {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "cpp"},
            {Litr::CLI::Instruction::Code::EXECUTE, "build.cpp"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Valid multiple nested command execution") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{R"(-t="debug" build cpp, java)"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 8> definition{
        {{Litr::CLI::Instruction::Code::DEFINE, "t"},
            {Litr::CLI::Instruction::Code::CONSTANT, "debug"},
            {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
            {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "cpp"},
            {Litr::CLI::Instruction::Code::EXECUTE, "build.cpp"},
            {Litr::CLI::Instruction::Code::CLEAR, NO_VALUE},
            {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "java"},
            {Litr::CLI::Instruction::Code::EXECUTE, "build.java"}}};

    CHECK_FALSE(parser.has_errors());
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Unsupported characters") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{"(9)"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse: Unexpected character.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Unsupported string between commands") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{R"(build "debug" project )"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `\"debug\"`: This is not allowed here.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Unsupported number between commands") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{R"(build 23 project )"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `23`: This is not allowed here.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Unsupported characters between commands") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{R"(build ++ project )"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse: Unexpected character.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Assignment missing parameter") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{R"(= "value")"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message,
        "Cannot parse at `=`: You are missing a parameter in front of the assignment.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Duplicated assignment operator") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{R"(-t == "value")"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(errors[0].message, "Cannot parse at `=`: Value assignment missing.");
    Litr::Error::Handler::flush();
  }

  TEST_CASE("Duplicated parameter initializer") {
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const std::string source{"---t"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.has_errors());

    const auto errors{Litr::Error::Handler::get_errors()};
    CHECK_EQ(errors.size(), 1);
    CHECK_EQ(
        errors[0].message, "Cannot parse: A parameter can only start with the characters A-Za-z.");
    Litr::Error::Handler::flush();
  }
}
