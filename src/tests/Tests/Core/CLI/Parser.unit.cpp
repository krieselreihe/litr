#include <doctest/doctest.h>
#include <fmt/format.h>

#include "Core/CLI/Parser.hpp"

#define NO_VALUE ""
#define CHECK_DEFINITION(instruction, definition)                                           \
  {                                                                                         \
    size_t iteration{0};                                                                    \
    size_t offset{0};                                                                       \
    while (offset < (instruction)->Count()) {                                               \
      const auto test{(definition)[iteration]};                                             \
      const auto code{static_cast<Litr::Instruction::Code>((instruction)->Read(offset++))}; \
      switch (code) {                                                                       \
        case Litr::Instruction::Code::CONSTANT:                                             \
        case Litr::Instruction::Code::DEFINE:                                               \
        case Litr::Instruction::Code::BEGIN_SCOPE:                                          \
        case Litr::Instruction::Code::EXECUTE: {                                            \
          const std::byte index{(instruction)->Read(offset)};                               \
          const Litr::Instruction::Value constant{(instruction)->ReadConstant(index)};      \
          offset += 1;                                                                      \
          CHECK(test.Code == code);                                                         \
          CHECK(test.Value == constant);                                                    \
          break;                                                                            \
        }                                                                                   \
        case Litr::Instruction::Code::CLEAR: {                                              \
          CHECK(test.Code == code);                                                         \
          break;                                                                            \
        }                                                                                   \
        default:                                                                            \
          CHECK_MESSAGE(false, fmt::format("Unknown Instruction::Code {:d}", code));        \
          offset += 1;                                                                      \
      }                                                                                     \
      iteration++;                                                                          \
    }                                                                                       \
    CHECK(iteration == (definition).size());                                                \
  }

struct InstructionDefinition {
  Litr::Instruction::Code Code;
  Litr::Instruction::Value Value;
} __attribute__((aligned(32)));

TEST_SUITE("Parser") {
  TEST_CASE("Single long parameter") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{R"(--target="Some release")"};

    Litr::Parser parser{errorHandler, instruction, source};
    const std::array<InstructionDefinition, 2> definition{{
        {Litr::Instruction::Code::DEFINE, "target"},
        {Litr::Instruction::Code::CONSTANT, "Some release"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
  }

  TEST_CASE("Single short parameter") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{R"(-t="debug is nice")"};

    Litr::Parser parser{errorHandler, instruction, source};
    const std::array<InstructionDefinition, 2> definition{{
        {Litr::Instruction::Code::DEFINE, "t"},
        {Litr::Instruction::Code::CONSTANT, "debug is nice"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
  }

  TEST_CASE("Single command") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{"build"};

    Litr::Parser parser{errorHandler, instruction, source};
    const std::array<InstructionDefinition, 2> definition{{
        {Litr::Instruction::Code::BEGIN_SCOPE, "build"},
        {Litr::Instruction::Code::EXECUTE, "build"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
  }

  TEST_CASE("Multiple commands") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{"build cpp"};

    Litr::Parser parser{errorHandler, instruction, source};
    const std::array<InstructionDefinition, 3> definition{{
        {Litr::Instruction::Code::BEGIN_SCOPE, "build"},
        {Litr::Instruction::Code::BEGIN_SCOPE, "cpp"},
        {Litr::Instruction::Code::EXECUTE, "build.cpp"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
  }

  TEST_CASE("Multiple comma separated commands") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{"build,run"};

    Litr::Parser parser{errorHandler, instruction, source};
    const std::array<InstructionDefinition, 5> definition{{
        {Litr::Instruction::Code::BEGIN_SCOPE, "build"},
        {Litr::Instruction::Code::EXECUTE, "build"},
        {Litr::Instruction::Code::CLEAR, NO_VALUE},
        {Litr::Instruction::Code::BEGIN_SCOPE, "run"},
        {Litr::Instruction::Code::EXECUTE, "run"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
  }

  TEST_CASE("Long parameter with string values and commands") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{R"(--target="release" build,run)"};

    Litr::Parser parser{errorHandler, instruction, source};
    const std::array<InstructionDefinition, 7> definition{{
        {Litr::Instruction::Code::DEFINE, "target"},
        {Litr::Instruction::Code::CONSTANT, "release"},
        {Litr::Instruction::Code::BEGIN_SCOPE, "build"},
        {Litr::Instruction::Code::EXECUTE, "build"},
        {Litr::Instruction::Code::CLEAR, NO_VALUE},
        {Litr::Instruction::Code::BEGIN_SCOPE, "run"},
        {Litr::Instruction::Code::EXECUTE, "run"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
  }

  TEST_CASE("Mixed parameters with mixed values") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{R"(-t="release" --debug)"};

    Litr::Parser parser{errorHandler, instruction, source};
    const std::array<InstructionDefinition, 3> definition{{
        {Litr::Instruction::Code::DEFINE, "t"},
        {Litr::Instruction::Code::CONSTANT, "release"},
        {Litr::Instruction::Code::DEFINE, "debug"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
  }

  TEST_CASE("Invalid comma operator") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{","};

    Litr::Parser parser{errorHandler, instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{errorHandler->GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at ',': Unexpected comma.\n");
  }

  TEST_CASE("Invalid comma operators with parameter") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{R"(-t="debug" ,)"};

    Litr::Parser parser{errorHandler, instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{errorHandler->GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at ',': Unexpected comma.\n");
  }

  TEST_CASE("Invalid comma operators with command") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{"run ,,"};

    Litr::Parser parser{errorHandler, instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{errorHandler->GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at ',': Duplicated comma.\n");
  }

  TEST_CASE("Invalid multiple comma operators") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{", ,"};

    Litr::Parser parser{errorHandler, instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{errorHandler->GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at ',': Unexpected comma.\n");
  }

  TEST_CASE("Invalid multiple comma operators with commands") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{"build cpp , ,"};

    Litr::Parser parser{errorHandler, instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{errorHandler->GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at ',': Duplicated comma.\n");
  }

  TEST_CASE("Even more invalid multiple comma operators with commands") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{"build cpp ,,,,"};

    Litr::Parser parser{errorHandler, instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{errorHandler->GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at ',': Duplicated comma.\n");
  }

  TEST_CASE("Invalid multiple comma operators with closing command") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{"build cpp , , run"};

    Litr::Parser parser{errorHandler, instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{errorHandler->GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at ',': Duplicated comma.\n");
  }

  TEST_CASE("Valid nested command execution") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{R"(-t="debug" build cpp)"};

    Litr::Parser parser{errorHandler, instruction, source};
    const std::array<InstructionDefinition, 5> definition{{
        {Litr::Instruction::Code::DEFINE, "t"},
        {Litr::Instruction::Code::CONSTANT, "debug"},
        {Litr::Instruction::Code::BEGIN_SCOPE, "build"},
        {Litr::Instruction::Code::BEGIN_SCOPE, "cpp"},
        {Litr::Instruction::Code::EXECUTE, "build.cpp"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
  }

  TEST_CASE("Valid multiple nested command execution") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{R"(-t="debug" build cpp, java)"};

    Litr::Parser parser{errorHandler, instruction, source};
    const std::array<InstructionDefinition, 8> definition{{
        {Litr::Instruction::Code::DEFINE, "t"},
        {Litr::Instruction::Code::CONSTANT, "debug"},
        {Litr::Instruction::Code::BEGIN_SCOPE, "build"},
        {Litr::Instruction::Code::BEGIN_SCOPE, "cpp"},
        {Litr::Instruction::Code::EXECUTE, "build.cpp"},
        {Litr::Instruction::Code::CLEAR, NO_VALUE},
        {Litr::Instruction::Code::BEGIN_SCOPE, "java"},
        {Litr::Instruction::Code::EXECUTE, "build.java"}
    }};

    CHECK(parser.HasErrors() == false);
    CHECK_DEFINITION(instruction, definition);
  }

  TEST_CASE("Unsupported characters") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{"(9)"};

    Litr::Parser parser{errorHandler, instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{errorHandler->GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse: Unexpected character.\n");
  }

  TEST_CASE("Unsupported string between commands") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{R"(build "debug" project )"};

    Litr::Parser parser{errorHandler, instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{errorHandler->GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at '\"debug\"': This is not allowed here.\n");
  }

  TEST_CASE("Unsupported number between commands") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{R"(build 23 project )"};

    Litr::Parser parser{errorHandler, instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{errorHandler->GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at '23': This is not allowed here.\n");
  }

  TEST_CASE("Unsupported characters between commands") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{R"(build ++ project )"};

    Litr::Parser parser{errorHandler, instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{errorHandler->GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse: Unexpected character.\n");
  }

  TEST_CASE("Assignment missing parameter") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{R"(= "value")"};

    Litr::Parser parser{errorHandler, instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{errorHandler->GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at '=': You are missing a parameter in front of the assignment.\n");
  }

  TEST_CASE("Duplicated assignment operator") {
    const auto errorHandler{Litr::CreateRef<Litr::ErrorHandler>()};
    const auto instruction{Litr::CreateRef<Litr::Instruction>()};
    const std::string source{R"(-t == "value")"};

    Litr::Parser parser{errorHandler, instruction, source};

    CHECK(parser.HasErrors() == true);

    const auto errors{errorHandler->GetErrors()};
    CHECK(errors.size() == 1);
    CHECK(errors[0].Message == "Cannot parse at '=': Value assignment missing.\n");
  }
}
