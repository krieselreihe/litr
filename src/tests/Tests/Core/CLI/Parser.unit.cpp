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
      const auto code{static_cast<Litr::CLI::Instruction::Code>((instruction)->Read(offset++))}; \
      switch (code) {                                                                            \
        case Litr::CLI::Instruction::Code::CONSTANT:                                             \
        case Litr::CLI::Instruction::Code::DEFINE:                                               \
        case Litr::CLI::Instruction::Code::BEGIN_SCOPE:                                          \
        case Litr::CLI::Instruction::Code::EXECUTE: {                                            \
          const std::byte index{(instruction)->Read(offset)};                                    \
          const Litr::CLI::Instruction::Value constant{(instruction)->ReadConstant(index)};      \
          offset += 1;                                                                           \
          CHECK(test.Code == code);                                                              \
          CHECK(test.Value == constant);                                                         \
          break;                                                                                 \
        }                                                                                        \
        case Litr::CLI::Instruction::Code::CLEAR: {                                              \
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
  Litr::CLI::Instruction::Code Code;
  Litr::CLI::Instruction::Value Value;
};

TEST_SUITE("CLI::Parser") {
  TEST_CASE("Single long parameter") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{R"(--target="Some release")"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 2> definition{{
        {Litr::CLI::Instruction::Code::DEFINE, "target"},
        {Litr::CLI::Instruction::Code::CONSTANT, "Some release"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Single short parameter") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{R"(-t="debug is nice")"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 2> definition{{
        {Litr::CLI::Instruction::Code::DEFINE, "t"},
        {Litr::CLI::Instruction::Code::CONSTANT, "debug is nice"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Parameter with empty string") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{R"(-t="")"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 2> definition{{
        {Litr::CLI::Instruction::Code::DEFINE, "t"},
        {Litr::CLI::Instruction::Code::CONSTANT, ""}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Single command") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{"build"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 2> definition{{
        {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
        {Litr::CLI::Instruction::Code::EXECUTE, "build"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Multiple commands") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{"build cpp"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 3> definition{{
        {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
        {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "cpp"},
        {Litr::CLI::Instruction::Code::EXECUTE, "build.cpp"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Multiple comma separated commands") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{"build,run"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 5> definition{{
        {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
        {Litr::CLI::Instruction::Code::EXECUTE, "build"},
        {Litr::CLI::Instruction::Code::CLEAR, NO_VALUE},
        {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "run"},
        {Litr::CLI::Instruction::Code::EXECUTE, "run"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Long parameter with string values and commands") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{R"(--target="release" build,run)"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 7> definition{{
        {Litr::CLI::Instruction::Code::DEFINE, "target"},
        {Litr::CLI::Instruction::Code::CONSTANT, "release"},
        {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
        {Litr::CLI::Instruction::Code::EXECUTE, "build"},
        {Litr::CLI::Instruction::Code::CLEAR, NO_VALUE},
        {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "run"},
        {Litr::CLI::Instruction::Code::EXECUTE, "run"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Mixed parameters with mixed values") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{R"(-t="release" --debug)"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 3> definition{{
        {Litr::CLI::Instruction::Code::DEFINE, "t"},
        {Litr::CLI::Instruction::Code::CONSTANT, "release"},
        {Litr::CLI::Instruction::Code::DEFINE, "debug"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Invalid comma operator") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{","};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{Litr::Error::Handler::GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at `,`: Unexpected comma.\n");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Invalid comma operators with parameter") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{R"(-t="debug" ,)"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{Litr::Error::Handler::GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at `,`: Unexpected comma.\n");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Invalid comma operators with command") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{"run ,,"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{Litr::Error::Handler::GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at `,`: Duplicated comma.\n");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Invalid multiple comma operators") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{", ,"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{Litr::Error::Handler::GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at `,`: Unexpected comma.\n");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Invalid multiple comma operators with commands") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{"build cpp , ,"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{Litr::Error::Handler::GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at `,`: Duplicated comma.\n");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Even more invalid multiple comma operators with commands") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{"build cpp ,,,,"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{Litr::Error::Handler::GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at `,`: Duplicated comma.\n");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Invalid multiple comma operators with closing command") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{"build cpp , , run"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{Litr::Error::Handler::GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at `,`: Duplicated comma.\n");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Valid nested command execution") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{R"(-t="debug" build cpp)"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 5> definition{{
        {Litr::CLI::Instruction::Code::DEFINE, "t"},
        {Litr::CLI::Instruction::Code::CONSTANT, "debug"},
        {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
        {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "cpp"},
        {Litr::CLI::Instruction::Code::EXECUTE, "build.cpp"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Valid multiple nested command execution") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{R"(-t="debug" build cpp, java)"};

    Litr::CLI::Parser parser{instruction, source};
    const std::array<InstructionDefinition, 8> definition{{
        {Litr::CLI::Instruction::Code::DEFINE, "t"},
        {Litr::CLI::Instruction::Code::CONSTANT, "debug"},
        {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "build"},
        {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "cpp"},
        {Litr::CLI::Instruction::Code::EXECUTE, "build.cpp"},
        {Litr::CLI::Instruction::Code::CLEAR, NO_VALUE},
        {Litr::CLI::Instruction::Code::BEGIN_SCOPE, "java"},
        {Litr::CLI::Instruction::Code::EXECUTE, "build.java"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Unsupported characters") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{"(9)"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{Litr::Error::Handler::GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse: Unexpected character.\n");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Unsupported string between commands") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{R"(build "debug" project )"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{Litr::Error::Handler::GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at `\"debug\"`: This is not allowed here.\n");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Unsupported number between commands") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{R"(build 23 project )"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{Litr::Error::Handler::GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at `23`: This is not allowed here.\n");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Unsupported characters between commands") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{R"(build ++ project )"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{Litr::Error::Handler::GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse: Unexpected character.\n");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Assignment missing parameter") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{R"(= "value")"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{Litr::Error::Handler::GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at `=`: You are missing a parameter in front of the assignment.\n");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Duplicated assignment operator") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{R"(-t == "value")"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{Litr::Error::Handler::GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at `=`: Value assignment missing.\n");
    Litr::Error::Handler::Flush();
  }

  TEST_CASE("Duplicated parameter initializer") {
    const auto instruction{Litr::CreateRef<Litr::CLI::Instruction>()};
    const std::string source{"---t"};

    Litr::CLI::Parser parser{instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{Litr::Error::Handler::GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse: A parameter can only start with the characters A-Za-z.\n");
    Litr::Error::Handler::Flush();
  }
}
