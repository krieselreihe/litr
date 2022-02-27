/*
 * Copyright (c) 2020-2022 Martin Helmut Fieber <info@martin-fieber.se>
 */

#include <doctest/doctest.h>

#include "Core/Config/FileResolver.hpp"

TEST_SUITE("Config::FileResolver") {
  TEST_CASE("File not found" * doctest::skip()) {
    // @todo: This will find a potential config in home of the executing system.
    // So mocking would be solution, maybe.
    litr::Config::FileResolver config{"/some/path/to/nowhere"};
    CHECK_EQ(config.GetStatus(), litr::Config::FileResolver::Status::NOT_FOUND);
    CHECK_EQ(config.GetFilePath(), "");
  }
}
