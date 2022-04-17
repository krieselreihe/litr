/*
 * Copyright (c) 2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include <doctest/doctest.h>

#include <memory>

#include "Client/Hooks/Help/NewHelp.hpp"
#include "Core.hpp"

TEST_SUITE("Hooks::Help") {
  TEST_CASE("Show help for all available commands") {
    const auto config{std::make_shared<Litr::Config::Loader>(
        Litr::Path("../../Fixtures/Config/full-example.toml"))};
    const auto instruction{std::make_shared<Litr::CLI::Instruction>()};
    const Litr::CLI::Parser parser{instruction, "--help"};

    Litr::Hook::NewHelp help{config, instruction, false};

    CHECK_EQ(help.read(),
        "Litr - Language Independent Task Runner [version 1.0.0-alpha.3]\n"
        "  Configuration file found under: ../../Fixtures/Config/full-example.toml\n"
        "\n"
        "Usage: litr command [options]\n"
        "\n"
        "Commands:\n"
        "  release\n"
        "  ├ brew       [--debug] [--nolog] [--profile] [--release] [--target=<option>] [--trace]\n"
        "  │                      Build a production release for Homebrew on macOS.\n"
        "  └ snapcraft            Build a production release for Snapcraft on Linux.\n"
        "  build        [--debug] [--nolog] [--profile] [--release] [--target=<option>] [--trace]\n"
        "                         Build the application for a given target.\n"
        "  format                 Format project sources via clang-format.\n"
        "  update                 Update repository code.\n"
        "  test         [--target=<option>]\n"
        "                         Run all unit tests defined under src/tests\n"
        "\n"
        "Options:\n"
        "  -h --help              Show this screen.\n"
        "  -v --version           Show current Litr version.\n"
        "     --release           Create a release build detached from build target.\n"
        "  -l --nolog             Deactivate all logging output.\n"
        "  -t --something=<option>\n"
        "                         Define something for a test to see how things are aligned.\n"
        "                         Available options: \"one\", \"two\", \"three\", \"four\"\n"
        "                         Default option is: \"one\"\n"
        "  -d --debug             Set debug mode, even if build type differs.\n"
        "  -r --trace             Activate tracing support and parser disassemble output.\n"
        "  -p --profile           Build the application with profiling tools enabled.\n"
        "     --target=<option>   Define the application build target.\n"
        "                         Available options: \"debug\", \"release\"\n"
        "                         Default option is: \"debug\"");
  }
}
